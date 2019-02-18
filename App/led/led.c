#include "led.h"
#include "user.h"
#include "stm32f10x.h"

void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//����APB2ʱ��
	
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);							//�ܽų�ʼ��	
	
	GPIO_Write(GPIOC, 0xFFFF);
}

void led_run(void)
{
	/*************�⺯���汾****************
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
	user_delay(65500);
	user_delay(65500);
	user_delay(65500);
	user_delay(65500);
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	user_delay(65500);
	user_delay(65500);
	user_delay(65500);
	user_delay(65500);
	*/
	/********λ�β���********
	led_0 = !led_0;
	//user_delay(600000);
	delay_ms(500);	//ϵͳ��ʱ����ʱ500ms
	*************************/
	/*************�⺯���汾***************/
	int i;
	
	for (i = 0; i < 8; i++){
		led_n(i) = !led_n(i);
		delay_ms(500);
	}
}

