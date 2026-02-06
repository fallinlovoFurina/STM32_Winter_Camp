#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void Serial_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//开启外设时钟  USART1是APB2的外设
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX  对应蓝牙的接收
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//GPIO初始化函数  
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//GPIO初始化函数  
	
	USART_InitTypeDef USART_Initstructure;
	USART_Initstructure.USART_BaudRate=115200;//波特率
	USART_Initstructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//非硬件触发
	USART_Initstructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//输出和接收
	USART_Initstructure.USART_Parity=USART_Parity_No;//无校验
	USART_Initstructure.USART_StopBits=USART_StopBits_1;//一位停止位
	USART_Initstructure.USART_WordLength=USART_WordLength_8b;//八位字长
	USART_Init(USART1,&USART_Initstructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启RXNE标志位到NVIC的输出,通过接收寄存器来判断是否接收。

	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==0);//发送数据位空标志位
}

void Serial_SendArray(uint8_t *Array,uint16_t Length)//定义一个数组和长度
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);//将数组的每一项通过Serial_SendByte()发送
		
	}
	
}

void Serial_SendString(char *string)//发送字符串  字符串自带一个结束标志位
{
	uint8_t i;
	for(i=0;string[i]!=0;i++)//0为空字符 
	{
		Serial_SendByte(string[i]);
	}
	
}

uint32_t serial_Pow(uint32_t X,uint32_t Y)
{
	uint32_t Result=1;
	while(Y--)
	{
		Result*=X;
		
	}
	return  Result;
}

void Serial_SendNumber(uint32_t Number,uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Number/serial_Pow(10,Length-i-1)%10+0x30);//加上偏移量才能以字符显示
	}//先取高位
}

int fputc(int ch,FILE*f)//fputc为printf函数的底层
{
	Serial_SendByte(ch);//fputc 重定向到串口
	return ch;
}
void Serial_Printf(char *format, ...)//printf函数移植
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

//void USART1_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)//RXNE置1
//	{
//		Serial_RxData = USART_ReceiveData(USART1);//接受数据
//		Serial_RxFlag = 1;
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清除标志位
//	}
//}
