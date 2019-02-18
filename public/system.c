#include "system.h"
#include "stm32f10x_iwdg.h"
#include "led.h"

void IWDG_Init(uint8_t pre, uint16_t rlr)
{
	//cancel write register protected
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//set driver
	IWDG_SetPrescaler(pre);	//pre <= 0xFFF
	
	//set reload value
	IWDG_SetReload(rlr);
	
	//reload value	write 0xAAAA to IWDG_KR
	IWDG_ReloadCounter();

	//enable iwdg
	IWDG_Enable();
}

void IWDG_FeedDog(void)
{
	IWDG_ReloadCounter();
}

void WWDG_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//ʹ�ܿ��Ź�����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	//�����ϴ���ֵ
	WWDG_SetWindowValue(0x5F);
	
	//���ô��ڿ��Ź���Ƶֵ
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	
	//���ô��ڿ��Ź�NVIC
	NVIC_InitStruct.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//ʹ�ܴ��ڿ��Ź�����������ʼ����ʼ����ֵ
	WWDG_Enable(0x7F);
	
	//������ڿ��Ź�״̬��־
	WWDG_ClearFlag();
	
	//ʹ�ܴ��ڿ��Ź��ж�
	WWDG_EnableIT();
}

void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(0x7F);	//��װ�ؼ���ֵ
	WWDG_ClearFlag();

	led_n(7) = !led_n(7);
}



