#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include "PID.h"
#include "Timer.h"

//编码器TIM3
//定时器中断TIM2
//PWM是TIM4通道三
uint16_t Data1;
PID_TypeDef Inc;
float target;
int main(void)
{
	Encoder_Init();
	Serial_Init();
	Motor_Init();
	Timer_Init();
	target = 300 * 0.2;
	Incremental_PID_Init(&Inc, 1.0, 0.1, 0.05, 100, -100);//kp,ki,kd,max,min
	
	while (1)
	{
		int32_t pulse = Encoder_Get();
		Serial_Printf("pusle:%d\n",pulse);
		Delay_ms(1);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
//		float pulse = Encoder_Get()*0.2;
		int32_t pulse = Encoder_Get();
//        float out = Incremental_PID_Calculate(&Inc, target, pulse);
//        Motor_SetSpeed(out);
        
        // 输出更多调试信息
//        Serial_Printf("target:%.2f pusle:%.2f\n",target,pulse);
        Serial_Printf("pusle:%d\n",pulse);
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

