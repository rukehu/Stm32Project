#ifndef __USART_H
#define __USART_H
#include "user.h"

void USART1_Init(uint32_t bound);
void uart_out_process(void);
void uart_test_process(void);
void USART3_Init(uint32_t bound);
void USART2_Init(uint32_t bound);
char read_recev_fifo(void);
void USARTn_send_char(USART_TypeDef *USARTn, uint8_t ch);
#endif
