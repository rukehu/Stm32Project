#ifndef __DMA_H
#define __DMA_H
#include "user.h"
#include "stm32f10x_dma.h"

#define BUFF_LEN	7
extern uint32_t dma_buff[];
extern uint8_t p_buff[BUFF_LEN];

void Dma_Init(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DestAddr,
							uint32_t SrcAddr, uint16_t size);
void Dma_Enable(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t size);
void Dma_out_process(void);
#endif
