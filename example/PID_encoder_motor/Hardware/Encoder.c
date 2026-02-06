#include "stm32f10x.h"                  // Device header

void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65535 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM3,&TIM_ICInitStructure);
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_SetCounter(TIM3,0);
	TIM_Cmd(TIM3,ENABLE);
}

//int32_t Encoder_Get(void)
//{
//	int32_t Temp;
//	Temp = TIM_GetCounter(TIM3);
////	TIM_SetCounter(TIM3,0);
//	return Temp;
//}

int32_t Encoder_Get(void)
{
    static uint16_t last_count = 0;
    static int32_t total_count = 0;
    
    uint16_t current_count = TIM_GetCounter(TIM3);
    int16_t diff = (int16_t)(current_count - last_count);
    
    // 处理16位计数器溢出/下溢
    if (diff > 32767) {
        diff -= 65536;  // 下溢修正
    } else if (diff < -32768) {
        diff += 65536;  // 上溢修正
    }
    
    total_count += diff;
    last_count = current_count;
    
    return total_count;
}
