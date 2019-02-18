#include "user.h"
#include "misc.h"
#include "stm32f10x_tim.h"
#include "stdlib.h"
#include "stm32f10x_usart.h"

extern void USARTn_send_char(USART_TypeDef *USARTn, uint8_t ch);

static uint8_t	fac_us;
static uint16_t fac_ms;

void user_delay(uint32_t time)
{
	while (time--);
}

void SysTick_Init(uint8_t SYSCLK)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us = SYSCLK / 8;
	fac_ms = (uint16_t)fac_us * 1000;
}

void delay_us(uint32_t us)
{
	uint32_t temp;
	SysTick->LOAD = us * fac_us;
	SysTick->VAL = 0x00;		//clear systick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	do {
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16)));
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x00;
}

void delay_ms(uint32_t ms)
{
	uint32_t temp;
	SysTick->LOAD = (uint32_t)ms * fac_ms;
	SysTick->VAL = 0x00;		//clear systick
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	do {
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16)));
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x00;
}

int fputc(int ch, FILE *p)
{
	USARTn_send_char(USART2,(uint8_t)ch);
	
	return ch;
}

void NVIC_IRQCannel_Enable(uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel	= IRQChannel;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd	= ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
}
	

