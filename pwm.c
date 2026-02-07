#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define PWM_PERIOD 100

void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = PWM_PERIOD - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)
{
    if (Compare > PWM_PERIOD)
    {
        Compare = PWM_PERIOD;
    }
    TIM_SetCompare1(TIM3, Compare);
}

void PWM_SetCompare2(uint16_t Compare)
{
    if (Compare > PWM_PERIOD)
    {
        Compare = PWM_PERIOD;
    }
    TIM_SetCompare2(TIM3, Compare);
}

void PWM_SetCompare3(uint16_t Compare)
{
    if (Compare > PWM_PERIOD)
    {
        Compare = PWM_PERIOD;
    }
    TIM_SetCompare1(TIM4, Compare);
}

void PWM_SetCompare4(uint16_t Compare)
{
    if (Compare > PWM_PERIOD)
    {
        Compare = PWM_PERIOD;
    }
    TIM_SetCompare2(TIM4, Compare);
}

void PWM_Task(void)
{
}

uint16_t PWM_GetCompare1(void)
{
    return TIM_GetCapture1(TIM3);
}

uint16_t PWM_GetCompare2(void)
{
    return TIM_GetCapture2(TIM3);
}

uint16_t PWM_GetCompare3(void)
{
    return TIM_GetCapture1(TIM4);
}

uint16_t PWM_GetCompare4(void)
{
    return TIM_GetCapture2(TIM4);
}
