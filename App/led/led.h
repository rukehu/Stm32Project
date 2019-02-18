#ifndef __LED_H
#define __LED_H
#include "system.h"
#include "user.h"

#define LED_PIN	(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)

/*
uint8_t sgmcode[16] = {0x3F, 0x06, 0x5B, 0x4F,
					 0x66, 0x6D, 0x7D, 0x07,
					 0x7F, 0x6F, 0x77, 0x7C,
					 0x39, 0x5E, 0x79, 0x71};	//0~F code */

#define led_n(n) PCout(n)

void Led_Init(void);
void led_run(void);
#endif

