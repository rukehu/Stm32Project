#include "timer.h"
#include "led.h"
#include "stm32f10x_tim.h"

void Timer2_Init(uint16_t load, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//����timer2����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//��ʼ��timer2o����
	TIM_TimeBaseInitStructure.TIM_Period = load;	//��װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	//��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	//����timer2�ж�
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	//����жϹ���
	
	//NVIC timer2�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//����timer2
	TIM_Cmd(TIM2, ENABLE);
}

void TIM3_CH1_PWM_Init(uint16_t per, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	//������Ӧ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//�ܽŸ���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//timer3ʱ������
	
	//gpio�˿�ģʽ��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);//��timer3�Ƚ�����ܽ���ȫ��ӳ�䵽PC6
	
	//pwm�Ƚ������ʼ��
	TIM_TimeBaseInitStructure.TIM_Period = per;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = psc;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	//ʹ��Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);	//ENABLE timer3 Ԥװ�� register
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
	
	//����GPIOA����ʱ��,ʹ��TIM5ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	//GPIOA�˿�����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;//��������ģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//��ʱ��ģʽ����
	TIM_TimeBaseInitStruct.TIM_Period = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = arr;
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	//TIM5���벶������
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 0x00;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;	//������
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;	//ֱ��ӳ�䵽TI1
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	TIM_ITConfig(TIM5, TIM_IT_CC1 | TIM_IT_Update, ENABLE);
	
	//TIM5 NVIC Init
	NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	//������ʱ��
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
