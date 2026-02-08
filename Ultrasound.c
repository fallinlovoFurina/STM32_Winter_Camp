#include "stm32f10x.h" // Device header
#include "Delay.h"

uint16_t Cnt;
uint16_t OverCnt;
void Ultrasound_Init()
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // trig
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // echo
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_InternalClockConfig(TIM1);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 59999; // ARR
    TIM_TimeBaseInitStructure.TIM_Prescaler = 71; // PSC
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
}
float Test_Distance()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    Delay_us(20);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == RESET)
    {
    };
    TIM_Cmd(TIM1, ENABLE);
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == SET)
    {
    };
    TIM_Cmd(TIM1, DISABLE);
    Cnt = TIM_GetCounter(TIM1);
    float distance = (Cnt * 1.0 / 10 * 0.34) / 2;
    TIM1->CNT = 0;
    Delay_ms(100);
    return distance;
}
