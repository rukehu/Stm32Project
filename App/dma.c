#include "dma.h"
#include "stm32f10x_rcc.h"
#include "usart.h"
#include "stm32f10x_usart.h"
#include "stdio.h"
#include "led.h"
#include "user.h"

uint32_t dma_buff[BUFF_LEN] = {0};
uint8_t p_buff[7] = {1, 2, 3, 4, 5, 6, 7};

void Dma_Init(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DestAddr,
							uint32_t SrcAddr, uint16_t size)
{
	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//DMA 配置初始化
	DMA_InitStruct.DMA_PeripheralBaseAddr = DestAddr;	//外设 address
	DMA_InitStruct.DMA_MemoryBaseAddr = SrcAddr;			//存储器地址
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//存储器到外设模式
	DMA_InitStruct.DMA_BufferSize = size;					//传输数据量
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//地址非增量模式
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;	//存储器增量模式
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设8位数据长度
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器8位数据长度
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//	DMA普通模式,非循环模式
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;//中等优先级模式
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//DMA通道没有开启存储器到存储器传输
	
	DMA_DeInit(DMAy_Channelx);
	DMA_Init(DMAy_Channelx, &DMA_InitStruct);
	
	//DMA发送完成中断配置
	DMA_ITConfig(DMAy_Channelx, DMA_IT_TC, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
}

void Dma_Enable(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t size)
{
	//DMA_Cmd(DMAy_Channelx, DISABLE);
	DMA_SetCurrDataCounter(DMAy_Channelx, size);	//数据传输量
	DMA_Cmd(DMAy_Channelx, ENABLE);
}

void Dma_out_process(void)
{
	uint8_t data, i;
	
	i = 0;
	while ((data = read_recev_fifo()) && (i < (BUFF_LEN-2))) {
		dma_buff[i] = data;
		i++;
	}
	
#if 0
	while (p[i]) {
		dma_buff[i] = p[i];
		i++;
	}
	
	if (0 != i) {
		while (i != BUFF_LEN-2){
			dma_buff[i] = 0x32;
			i++;
		}
		dma_buff[i] = 0x0D;
		i++;
		dma_buff[i] = 0x0A;
	}
#endif
	
	if (0 != i) {
		USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); 
		Dma_Enable(DMA1_Channel2, BUFF_LEN);
		//led_n(3) = !led_n(3);
	}
	
	return;
}

void DMA1_Channel2_IRQHandler(void)
{
	if (SET == DMA_GetFlagStatus(DMA1_FLAG_TC2)) {
		DMA_ClearFlag(DMA1_FLAG_TC2);
		//led_n(5) = !led_n(5);
	}
	
	DMA_Cmd(DMA1_Channel2, DISABLE);
}
