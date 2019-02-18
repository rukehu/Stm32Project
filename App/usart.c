#include "usart.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "user.h"
#include "led.h"

#define FIFO_MAX		256
uint8_t recev_fifo[FIFO_MAX] = {0};
uint8_t send_fifo[FIFO_MAX] = {0};
uint8_t Writ_RxFifo = 0;
uint8_t Read_RxFifo = 0;
uint8_t Writ_TxFifo = 0;
uint8_t Read_TxFifo = 0;

void USART1_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//开启总线复用时钟，开启usart1时钟，开启PA组管脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//输入输出端口复用配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//Tx pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		//Rx pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//floating mode input
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART1 init config
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_Init(USART1, &USART_InitStructure);
	
	//ENABLE USART1
	USART_Cmd(USART1, ENABLE);
	
	//USART1 INTERRUPT FLAG CLEAR
	//USART_ClearFlag(USART1, USART_FLAG_TC);	//清除数据发送完成标志
	
	//USART1 interrupt select and enable
	//USART_ITConfig(USART1, USART_IT_RXNE | USART_IT_TC, ENABLE);	//enable Rx recve and Tx send interrupt 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//*((uint32_t *)(0x20AC)) |= 0x40;
	
	//config USART1 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//channel interrupt enable
	NVIC_Init(&NVIC_InitStructure);
}

void USART3_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//开启总线复用时钟，开启usart3时钟，开启PB组管脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//输入输出端口复用配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		//Tx pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//Rx pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//floating mode input
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//USART3 init config
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_Init(USART3, &USART_InitStructure);
	
	//ENABLE USART3
	USART_Cmd(USART3, ENABLE);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	//config USART3 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//channel interrupt enable
	NVIC_Init(&NVIC_InitStructure);
}

void USART2_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//开启总线复用时钟，开启usart2时钟，开启PB组管脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//输入输出端口复用配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		//Tx pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		//Rx pin
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//floating mode input
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART2 init config
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_Init(USART2, &USART_InitStructure);
	
	//ENABLE USART2
	USART_Cmd(USART2, ENABLE);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_IRQCannel_Enable(USART2_IRQn, 3, 2);
}


void recev_callback(uint8_t ch)
{
	if ((Writ_RxFifo+1) % FIFO_MAX != Read_RxFifo){
		recev_fifo[Writ_RxFifo] = ch;
		Writ_RxFifo = (Writ_RxFifo + 1) % FIFO_MAX;
	}
}

char read_recev_fifo(void)
{
	uint8_t ch;
	if (Read_RxFifo != Writ_RxFifo) {
		ch = recev_fifo[Read_RxFifo];
		Read_RxFifo = (Read_RxFifo + 1) % FIFO_MAX;
		
		return ch;
	}

	return 0;
}

void USARTn_send_char(USART_TypeDef *USARTn, uint8_t ch)
{
	USART_SendData(USARTn, ch);
	while(RESET == USART_GetFlagStatus(USARTn, USART_FLAG_TXE));	//等待数据进入移位寄存器
}

void write_send_fifo(uint8_t ch)
{
	if ((Writ_TxFifo + 1) % FIFO_MAX != Read_TxFifo) {
		send_fifo[Writ_TxFifo] = ch;
		Writ_TxFifo = (Writ_TxFifo + 1) % FIFO_MAX;
	}

}

void uart_out_process(void)
{
	while (Read_TxFifo != Writ_TxFifo) {
		USARTn_send_char(USART2, send_fifo[Read_TxFifo]);
		Read_TxFifo = (Read_TxFifo + 1) % FIFO_MAX;
	}
}

void uart_test_process(void)
{
	uint8_t swp;
	while (Read_RxFifo != Writ_RxFifo) {
		swp = recev_fifo[Read_RxFifo];
		Read_RxFifo = (Read_RxFifo + 1) % FIFO_MAX;
		write_send_fifo(swp);
	}
}

void USART1_IRQHandler(void)
{
	if (SET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE)){
		recev_callback(USART_ReceiveData(USART1));	//(USART1->DR)recve data register
		//led_n(2) = !led_n(2);
	}
}

void USART3_IRQHandler(void)
{
	//led_n(6) = !led_n(6);
	if (SET == USART_GetFlagStatus(USART3, USART_FLAG_RXNE)){
		recev_callback(USART_ReceiveData(USART3));	//(USART1->DR)recve data register
		//led_n(2) = !led_n(2);
	}
}

void USART2_IRQHandler(void)
{
	//led_n(6) = !led_n(6);
	if (SET == USART_GetFlagStatus(USART2, USART_FLAG_RXNE)){
		recev_callback(USART_ReceiveData(USART2));	//(USART1->DR)recve data register
		//led_n(2) = !led_n(2);
	}
}
