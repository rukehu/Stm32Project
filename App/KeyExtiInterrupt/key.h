#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x_gpio.h"

#define KEY_UP		GPIO_Pin_0
#define KEY_LEFT	GPIO_Pin_2
#define KEY_DOWN	GPIO_Pin_3
#define KEY_RIGHT	GPIO_Pin_4

#define Key_UPin		PAin(0)
#define Key_LEFTin	PEin(2)
#define Key_DOWNin	PEin(3)
#define Key_RIGHTin	PEin(4)

void key_init(void);
void Key_Check(void);//°´¼ü¼ì²â
#endif

