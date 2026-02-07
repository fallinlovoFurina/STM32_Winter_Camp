#include "motor.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "delay.h"

/* * 硬件连接修正版:
 * 左电机 (A): PA0 (TIM2_CH1), PA1 (TIM2_CH2)
 * 右电机 (B): PA2 (TIM2_CH3), PA3 (TIM2_CH4)
 * 修正逻辑：全反向
 */

// 限制PWM范围在 0-100
static float limit_pwm(float pwm)
{
    if (pwm > 100)
        return 100;
    if (pwm < 0)
        return 0;
    return pwm;
}

// 延时辅助函数
static void move_delay(float cm, float speed)
{
    float time_ms = (cm * 60) / (speed / 10);
    if (speed <= 0)
        time_ms = 0;
    Delay_ms((uint16_t)time_ms);
}

void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = 99;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_Cmd(TIM2, ENABLE);
}

void Motor_Stop(void)
{
    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, 0);
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
}

// 【修正】前进：左轮(CH1转, CH2停)，右轮(CH3停, CH4转)
void Motor_Forward(float left_pwm, float right_pwm)
{
    left_pwm = limit_pwm(left_pwm);
    right_pwm = limit_pwm(right_pwm);

    // 左轮前进
    TIM_SetCompare1(TIM2, (uint16_t)left_pwm);
    TIM_SetCompare2(TIM2, 0);

    // 右轮前进
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, (uint16_t)right_pwm);
}

// 【修正】后退：左轮(CH1停, CH2转)，右轮(CH3转, CH4停)
void Motor_Back(float left_pwm, float right_pwm)
{
    left_pwm = limit_pwm(left_pwm);
    right_pwm = limit_pwm(right_pwm);

    // 左轮后退
    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, (uint16_t)left_pwm);

    // 右轮后退
    TIM_SetCompare3(TIM2, (uint16_t)right_pwm);
    TIM_SetCompare4(TIM2, 0);
}

// 【修正】左转：左轮不动，右轮前进
void Motor_Left(float right_pwm)
{
    right_pwm = limit_pwm(right_pwm);

    // 左轮停止
    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, 0);

    // 右轮前进
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, (uint16_t)right_pwm);
}

// 【修正】右转：左轮前进，右轮不动
void Motor_Right(float left_pwm)
{
    left_pwm = limit_pwm(left_pwm);

    // 左轮前进
    TIM_SetCompare1(TIM2, (uint16_t)left_pwm);
    TIM_SetCompare2(TIM2, 0);

    // 右轮停止
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
}

void Motor_Left_Brake(void)
{
    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, 0);
}

void Motor_Right_Brake(void)
{
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, 0);
}

void Motor_MoveForward(float cm, float left_speed, float right_speed)
{
    Motor_Forward(left_speed, right_speed);
    move_delay(cm, (left_speed + right_speed) / 2);
    Motor_Stop();
}

void Motor_MoveBack(float cm)
{
    Motor_Back(BACK_SPEED, BACK_SPEED);
    move_delay(cm, BACK_SPEED);
    Motor_Stop();
}

// 【修正】原地右转：左轮前进，右轮后退
void Motor_TurnRight90(void)
{
    // 左轮前
    TIM_SetCompare1(TIM2, (uint16_t)TURN_SPEED);
    TIM_SetCompare2(TIM2, 0);

    // 右轮后
    TIM_SetCompare3(TIM2, (uint16_t)TURN_SPEED);
    TIM_SetCompare4(TIM2, 0);

    Delay_ms(400);
    Motor_Stop();
}

// 【修正】原地左转：左轮后退，右轮前进
void Motor_TurnLeft90(void)
{
    // 左轮后
    TIM_SetCompare1(TIM2, 0);
    TIM_SetCompare2(TIM2, (uint16_t)TURN_SPEED);

    // 右轮前
    TIM_SetCompare3(TIM2, 0);
    TIM_SetCompare4(TIM2, (uint16_t)TURN_SPEED);

    Delay_ms(400);
    Motor_Stop();
}

void Motor_ResumeNormal(void)
{
    Motor_Forward(NORMAL_LEFT_SPEED, NORMAL_RIGHT_SPEED);
}

// 占位符
// uint8_t IRSensor_Detect(...) { ... }
// float Test_Distance(...) { ... }