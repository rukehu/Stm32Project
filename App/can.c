#include "can.h"
#include "user.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_can.h"
#include "misc.h"

//#define CAN_Rx_IRQ_Enable

/*
*tsjw:����ͬ����Ծʱ�䵥Ԫ;(CAN_SJW_1tq~CAN_SJW_4tq)
*tbs2:ʱ���2ʱ�䵥Ԫ;(CAN_BS2_1tq~CAN_BS2_8tq)
*tbs1:ʱ���1ʱ�䵥Ԫ;(CAN_BS1_1tq~CAN_BS1_16tq)
*brp:�����ʷ�Ƶ��;(1~1024)tq = brp * tpclk1;
*������ = Fpclk1 / ((tbs2+tbs1+1) * brp);
*mode:��ͨģʽ(CAN_Mode_Normal),����ģʽ(CAN_Mode_LoopBack)
*/
void CAN_Mod_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_InitTypeDef CAN_InitStruct;							//CAN��Ԫ��ʼ���ṹ��
	CAN_FilterInitTypeDef CAN_FilterInitStruct;	//CAN�������ṹ��
	
	//ʹ��canʱ�ӣ�ʹ���շ��ܽ�ʱ��,�ܽų�ʼ������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//can��Ԫ����
	CAN_InitStruct.CAN_TTCM = DISABLE;	//��ʱ�䴥��ͨ��ģʽ�ر�
	CAN_InitStruct.CAN_ABOM = DISABLE;	//����Զ����߹���ر�
	CAN_InitStruct.CAN_AWUM = DISABLE;	//˯��ģʽͨ��������ѹر�(���CAN->CMR��SLEEPλ)
	CAN_InitStruct.CAN_NART = ENABLE;		//ʹ�ñ����Զ�����
	CAN_InitStruct.CAN_RFLM = DISABLE;	//���Ĳ�����,������������ʱ���µĽ��Ḳ�Ǿɵ�
	CAN_InitStruct.CAN_TXFP = DISABLE;	//���ȼ��ɱ��ı�ʶ������
	CAN_InitStruct.CAN_Mode = mode;			//ģʽѡ��
	CAN_InitStruct.CAN_SJW  = tsjw;			//����ͬ����Ծ���(Tsjw)=tsjw+1��ʱ�䵥λ
	CAN_InitStruct.CAN_BS1	= tbs1;
	CAN_InitStruct.CAN_BS2	= tbs2;
	CAN_InitStruct.CAN_Prescaler = brp;	//���÷�Ƶϵ��
	
	CAN_Init(CAN1, &CAN_InitStruct);
	
	//����can������
	CAN_FilterInitStruct.CAN_FilterNumber	= 0;	//ѡ�������0
	CAN_FilterInitStruct.CAN_FilterMode		= CAN_FilterMode_IdMask;	//����������ģʽ
	CAN_FilterInitStruct.CAN_FilterScale	= CAN_FilterScale_32bit;	//32bit
	CAN_FilterInitStruct.CAN_FilterIdHigh	= 0x0000;	//32bit ID��λ
	CAN_FilterInitStruct.CAN_FilterIdLow	= 0x0000;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh	= 0x0000;	//32λID�����λ
	CAN_FilterInitStruct.CAN_FilterMaskIdLow	= 0x0000;	
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment	=	CAN_Filter_FIFO0;	//������0������FIFO0
	CAN_FilterInitStruct.CAN_FilterActivation	=	ENABLE;	//ʹ�ܹ�����
	
	CAN_FilterInit(&CAN_FilterInitStruct);
	
#ifdef CAN_Rx_IRQ_Enable
	//can�����ж�����,NVIC can�����ж�ʹ��
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	//FIFO0��Ϣ�Һ��ж�ʹ��
	NVIC_IRQCannel_Enable(USB_LP_CAN1_RX0_IRQn, 1, 0);
#endif

}

int8_t CAN1_Msg_Send(uint8_t *msg, uint8_t len)
{
	uint8_t mbox;
	uint16_t i = 0;
	CanTxMsg TxMessage;
	
	TxMessage.StdId = 0x12;	//��׼��ʶ��Ϊ0
	TxMessage.ExtId	= 0x12;	//������չ��ʶ��(29λ)
	TxMessage.IDE		=	0;		//ʹ����չ��ʶ��
	TxMessage.RTR		=	0;		//��Ϣ����Ϊ����֡,һ֡8λ
	TxMessage.DLC		=	len;	//��������֡��
	for (i = 0; i < len; i++)	//���͵���Ч������
		TxMessage.Data[i] = msg[i];
	
	//��������
	mbox = CAN_Transmit(CAN1, &TxMessage);
	
	i = 0;
	
	//�ȴ����ͽ���,��ȴ�0xFFF
	while ((CAN_TxStatus_Failed == CAN_TransmitStatus(CAN1, mbox)) && (i <= 0xFFF))
		i++;
	
	return ((i > 0xFFF) ? -1 : 0);
}

uint8_t CAN1_Msg_Receive(uint8_t *buf)
{
	uint32_t i;
	CanRxMsg RxMessage;
	
	RxMessage.DLC = 0;
	
	if(0 != CAN_MessagePending(CAN1,CAN_FIFO0)) {	//���յ�����
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		for (i = 0; i < RxMessage.DLC; i++) {
			buf[i] = RxMessage.Data[i];
		}
	}
	
	return RxMessage.DLC;	//���ؽ��յ����ֽ�����0��ʾδ���յ�����
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
