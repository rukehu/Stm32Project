#include "key.h"
#include "led.h"
#include "stm32f10x_rcc.h"

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = KEY_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//下拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY_LEFT | KEY_DOWN | KEY_RIGHT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void Key_Check(void)
{
	if (1 == Key_UPin || 0 == Key_LEFTin || 0 == Key_DOWNin || 0 == Key_RIGHTin){
		delay_ms(10);
		if (1 == Key_UPin) {
			led_n(0) = !led_n(0);
		} else if (0 == Key_LEFTin){
			led_n(1) = !led_n(1);
		} else if (0 == Key_DOWNin){
			led_n(2) = !led_n(2);
		} else if (0 == Key_RIGHTin){
			led_n(3) = !led_n(3);
		}
	}
	
	return ;
}
