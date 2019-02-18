#include "beep.h"
#include "system.h"

#define BEEP_Out	PBout(5)

void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void beep_uadio(uint8_t KHz)
{
	uint8_t load;
	load = (1000 / KHz) / 2;
	delay_us(load);
	BEEP_Out = !BEEP_Out;
}
