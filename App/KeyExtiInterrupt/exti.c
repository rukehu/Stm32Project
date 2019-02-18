#include "user.h"
#include "key.h"
#include "led.h"

void Key_Exit_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	//开启端口复用时钟AFIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//映射GPIO端口引脚为中断输入引脚
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);//select PA0为外部中断0的输入
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
	
	//NVIC中断通道初始化
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;	//select外部中断通道0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//开启中断
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//外部中断配置
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;				//select0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//mode select interrupt
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//使能中断
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//select interrupt mode
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	if (1 == EXTI_GetITStatus(EXTI_Line0)){	//获取中断标志位
		delay_ms(10);
		if (1 == Key_UPin)
			led_n(0) = !led_n(0);
	}

	EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI2_IRQHandler(void)
{
	delay_ms(10);
	if (0 == Key_LEFTin)
		led_n(2) = !led_n(2);
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler(void)
{
	delay_ms(10);
	if (0 == Key_DOWNin)
		led_n(3) = !led_n(3);
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);
	if (0 == Key_RIGHTin)
		led_n(4) = !led_n(4);
	EXTI_ClearITPendingBit(EXTI_Line4);
}

