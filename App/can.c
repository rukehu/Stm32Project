#include "can.h"
#include "user.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "misc.h"

//#define CAN_Rx_IRQ_Enable

/*
*tsjw:重新同步跳跃时间单元;(CAN_SJW_1tq~CAN_SJW_4tq)
*tbs2:时间段2时间单元;(CAN_BS2_1tq~CAN_BS2_8tq)
*tbs1:时间段1时间单元;(CAN_BS1_1tq~CAN_BS1_16tq)
*brp:波特率分频器;(1~1024)tq = brp * tpclk1;
*波特率 = Fpclk1 / ((tbs2+tbs1+1) * brp);
*mode:普通模式(CAN_Mode_Normal),环回模式(CAN_Mode_LoopBack)
*/
void CAN_Mod_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_InitTypeDef CAN_InitStruct;							//CAN单元初始化结构体
	CAN_FilterInitTypeDef CAN_FilterInitStruct;	//CAN过滤器结构体
	
	//使能can时钟，使能收发管脚时钟,管脚初始化配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//can单元配置
	CAN_InitStruct.CAN_TTCM = DISABLE;	//非时间触发通信模式关闭
	CAN_InitStruct.CAN_ABOM = DISABLE;	//软件自动离线管理关闭
	CAN_InitStruct.CAN_AWUM = DISABLE;	//睡眠模式通过软件唤醒关闭(清除CAN->CMR的SLEEP位)
	CAN_InitStruct.CAN_NART = ENABLE;		//使用报文自动传送
	CAN_InitStruct.CAN_RFLM = DISABLE;	//报文不锁定,当缓冲区满的时候新的将会覆盖旧的
	CAN_InitStruct.CAN_TXFP = DISABLE;	//优先级由报文标识符决定
	CAN_InitStruct.CAN_Mode = mode;			//模式选择
	CAN_InitStruct.CAN_SJW  = tsjw;			//重新同步跳跃宽度(Tsjw)=tsjw+1个时间单位
	CAN_InitStruct.CAN_BS1	= tbs1;
	CAN_InitStruct.CAN_BS2	= tbs2;
	CAN_InitStruct.CAN_Prescaler = brp;	//设置分频系数
	
	CAN_Init(CAN1, &CAN_InitStruct);
	
	//配置can过滤器
	CAN_FilterInitStruct.CAN_FilterNumber	= 0;	//选择过滤器0
	CAN_FilterInitStruct.CAN_FilterMode		= CAN_FilterMode_IdMask;	//过滤器掩码模式
	CAN_FilterInitStruct.CAN_FilterScale	= CAN_FilterScale_32bit;	//32bit
	CAN_FilterInitStruct.CAN_FilterIdHigh	= 0x0000;	//32bit ID高位
	CAN_FilterInitStruct.CAN_FilterIdLow	= 0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh	= 0x0000;	//32位ID掩码高位
	CAN_FilterInitStruct.CAN_FilterMaskIdLow	= 0x0000;	
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO0;	//过滤器0关联到FIFO0
	CAN_FilterInitStruct.CAN_FilterActivation	=	ENABLE;	//使能过滤器
	
	CAN_FilterInit(&CAN_FilterInitStruct);
	
#ifdef CAN_Rx_IRQ_Enable
	//can接收中断配置,NVIC can接收中断使能
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	//FIFO0消息挂号中断使能
	NVIC_IRQCannel_Enable(USB_LP_CAN1_RX0_IRQn, 1, 0);
#endif

}

int8_t CAN1_Msg_Send(uint8_t *msg, uint8_t len)
{
	uint8_t mbox;
	uint16_t i = 0;
	CanTxMsg TxMessage;
	
	TxMessage.StdId = 0x12;	//标准标识符为0
	TxMessage.ExtId	= 0x12;	//设置拓展标识符(29位)
	TxMessage.IDE		=	0;		//使用拓展标识符
	TxMessage.RTR		=	0;		//消息类型为数据帧,一帧8位
	TxMessage.DLC		=	len;	//发送数据帧数
	for (i = 0; i < len; i++)	//发送的有效数据域
		TxMessage.Data[i] = msg[i];
	
	//发送数据
	mbox = CAN_Transmit(CAN1, &TxMessage);
	
	i = 0;
	
	//等待发送结束,最长等待0xFFF
	while ((CAN_TxStatus_Failed == CAN_TransmitStatus(CAN1, mbox)) && (i <= 0xFFF))
		i++;
	
	return ((i > 0xFFF) ? -1 : 0);
}

uint8_t CAN1_Msg_Receive(uint8_t *buf)
{
	uint32_t i;
	CanRxMsg RxMessage;
	
	RxMessage.DLC = 0;
	
	if(0 != CAN_MessagePending(CAN1,CAN_FIFO0)) {	//接收到数据
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		for (i = 0; i < RxMessage.DLC; i++) {
			buf[i] = RxMessage.Data[i];
		}
	}
	
	return RxMessage.DLC;	//返回接收到的字节数，0表示未接收到数据
}

void can_test_process(void)
{
	uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7};
	
	if (0 == CAN1_Msg_Send(buf, 7)) {
		printf("send data success!\n");
	} else {
		printf("send data error!\n");
	}
	
}
