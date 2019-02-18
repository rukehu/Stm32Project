#include "timer.h"
#include "led.h"
#include "stm32f10x_tim.h"

void Timer2_Init(uint16_t load, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//开启timer2总线时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//初始化timer2o配置
	TIM_TimeBaseInitStructure.TIM_Period = load;	//重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	//分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	//开启timer2中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	//清除中断挂起
	
	//NVIC timer2中断设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//开启timer2
	TIM_Cmd(TIM2, ENABLE);
}

void TIM3_CH1_PWM_Init(uint16_t per, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	//开启对应总线时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//管脚复用时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//timer3时钟总线
	
	//gpio端口模式初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);//将timer3比较输出管脚完全重映射到PC6
	
	//pwm比较输出初始化
	TIM_TimeBaseInitStructure.TIM_Period = per;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	//使能预装载寄存器
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);	//ENABLE timer3 预装载 register
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//timer3 enable
	TIM_Cmd(TIM3, ENABLE);
}

void TIM5_CH1_Input_Init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//开启GPIOA总线时钟,使能TIM5时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	//GPIOA端口配置
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;//下拉输入模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//定时器模式配置
	TIM_TimeBaseInitStruct.TIM_Period = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	//TIM5输入捕获配置
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 0x00;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;	//捕获极性
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;	//直接映射到TI1
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	TIM_ITConfig(TIM5, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
	
	//TIM5 NVIC Init
	NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	//启动定时器
	TIM_Cmd(TIM5, ENABLE);
}

void Timer_Init(void)
{
	//Timer2_Init(1000, 36000 - 1);
	TIM3_CH1_PWM_Init(500, 72 - 1);
	TIM5_CH1_Input_Init(0xFFFF, 71);
	
	return ;
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)){
		led_n(5) = !led_n(5);
	}
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void TIM5_IRQHandler(void)
{
	
}
