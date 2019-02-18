#include "sysclock.h"
#include "user.h"
#include "app.h"

int main(void)
{
	//RCC_HSE_Config(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);	//36MHz
	SysTick_Init(72);	//ϵͳ�δ�ʱ�����ó�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	App_Init();
	
	AppProcess();
	
#if 0
	while (1)
	{
		//led_run();
//		delay_ms(500);
		//Key_Check();
		//beep_uadio(2);
	}
#endif	
}

