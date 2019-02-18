#ifndef __USER_H
#define __USER_H
#include <stdint.h>
#include "stm32f10x.h"
#include "stdio.h"

void user_delay(uint32_t time);
void SysTick_Init(uint8_t SYSCLK);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void NVIC_IRQCannel_Enable(uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority);

#endif
