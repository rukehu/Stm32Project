#include "led.h"
#include "user.h"
#include "exti.h"
#include "key.h"
#include "beep.h"
#include "timer.h"
#include "usart.h"
#include "system.h"
#include "stm32f10x_iwdg.h"
#include "dma.h"
#include "rtc.h"
#include "can.h"

void App_Init(void)
{
	Led_Init();
	//key_init();
	//Beep_Init();
	//Key_Exit_Init();
	//Timer_Init();
	IWDG_Init(4, 4095);	//0xFFF
	//WWDG_Init();
	//USART1_Init(9600);
	//USART3_Init(9600);
	USART2_Init(9600);
	//Dma_Init(DMA1_Channel2, (uint32_t)(&USART3->DR), (uint32_t)p_buff, (uint32_t)BUFF_LEN);
	//RTC_Init();
	//CAN_Mod_Init(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);
}

void AppProcess(void)
{
//	uint32_t i = 0, flag = 0;
	
	while (1)
	{
		IWDG_FeedDog();
		//TIM_SetCompare1(TIM3, i);
		uart_test_process();
		uart_out_process();
		//Dma_out_process();
		//show_rtc_date();
		//can_test_process();
		led_n(0) = !led_n(0);
		delay_ms(500);

		//printf("hello world!\r\n");
	}
}
