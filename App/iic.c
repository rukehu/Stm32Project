#include "iic.h"
#include "sysclock.h"

void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;	//IIC2_SCL ping
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;	//gpio_10 is SCL ping,gpio_11 is SDA ping
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	IIC_SCL_Out = 1;
	IIC_SDA_Out = 1;
}

/*
* @ before��IIC �������ģʽ��
* @ param ��Not
* @ return��Not
*/
void IIC_Out_Mode(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	//gpio_11 is SDA ping
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*
* @ before��IIC ��������ģʽ��
* @ param ��Not
* @ return��Not
*/
void IIC_In_Mode(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	//gpio_11 is SDA ping
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*
* @ before��IIC ������ʼ�źţ�
* @ param ��Not
* @ return��Not
*/
void IIC_Start_Signal(void)
{
	IIC_Out_Mode();
	
	IIC_SDA_Out = 1;
	IIC_SCL_Out = 1;
	delay_us(5);
	IIC_SDA_Out = 0;	//��SCLΪ��ʱ��SDA�ɸߵ��ף�������ʼ�ź�
	delay_us(6);
	IIC_SCL_Out = 0;	//��SCL���ͣ�׼�����ͻ��������
	
}

/*
* @ brief ��IIC ����ֹͣ�źţ�
* @ param ��Not
* @ return��Not
*/
void IIC_Stop_Signal(void)
{
	IIC_Out_Mode();
	
	IIC_SCL_Out = 0;
	IIC_SDA_Out = 0;
	IIC_SCL_Out = 1;	//SCL����ߵ�ƽ
	delay_us(6);
	IIC_SDA_Out = 1;	//SDA�ɵ͵���,������ֹ�ź�
	delay_us(5);
}

/*
* @ brief ��IIC �ȴ��ӻ�ACK��Ӧ��
* @ param ��Not
* @ return��0�ӻ���Ӧ��-1�ӻ�δ��Ӧ
*/
int8_t IIC_Wait_ACk(void)
{
	uint8_t wait = 0;
	
	IIC_In_Mode();
	
	IIC_SDA_Out = 1;
	delay_us(1);
	IIC_SCL_Out = 1;
	delay_us(1);
	
	while (IIC_SDA_In) {
		wait++;
		if (wait > 250) {
			IIC_Stop_Signal();	//�ӻ�δ��Ӧ����ֹ�˴����ݴ���
			return -1;
		}
	}
	IIC_SCL_Out = 0;	//SCL�ɸߵ��ף����һ��Ӧ��ʱ������
	
	return 0;
}

/*
* @ brief ��IIC�ӻ�ģʽ�£�����Ӧ���źţ�
* @ param ��Not
* @ return��Not
*/
void IIC_Send_ACK(void)
{
	IIC_SCL_Out = 0;
	IIC_Out_Mode();
	IIC_SDA_Out = 0;	//Ӧ��Ϊ�͵�ƽ
	delay_us(2);
	IIC_SCL_Out = 1;
	delay_us(5);
	IIC_SCL_Out = 0;	//IIC�ɸߵ��ڣ����һ��Ӧ��ʱ������	
}

/*
* @ brief ��IIC�ӻ�ģʽ�£����ͷ�Ӧ���źţ�
* @ param ��Not
* @ return��Not
*/
void IIC_Send_NACK(void)
{
	IIC_SCL_Out = 0;
	IIC_Out_Mode();
	IIC_SDA_Out = 1;	//Ӧ��Ϊ�ߵ�ƽ
	delay_us(2);
	IIC_SCL_Out = 1;
	delay_us(5);
	IIC_SCL_Out = 0;	//IIC�ɸߵ��ڣ����һ��Ӧ��ʱ������	
}

/*
* @ brief ��IIC����һ���ַ���
* @ param ��
*		data:���͵��ַ�
* @ return��Not
*/
void IIC_Send_Char(uint8_t data)
{
	uint8_t i;
	
	IIC_Out_Mode();
	IIC_SCL_Out = 0;	//׼����ʼ��������,�͵�ƽ��������
	
	for (i = 0; i < 8; i++) {
		if (data & 0x80) {
			IIC_SDA_Out = 1;
		} else {
			IIC_SDA_Out = 0;
		}
		data <<= 1;
		
		delay_us(2);
		IIC_SCL_Out = 1;	//�ߵ�ƽ���ݱ����ȶ�
		delay_us(2);
		IIC_SCL_Out = 0;	//�½��ز�������
		delay_us(2);
	}
}

/*
* @ brief ��IIC��ȡһ���ַ���
* @ param ��
*		ack:��ȡһ���ַ����͵�Ӧ�������0������Ӧ�𣩣�1��������Ӧ���������һ���ֽ����ݴ�����ϣ���
* @ return�����յ����ַ�
*/
uint8_t IIC_Receive_Char(uint8_t ack)
{
	uint8_t i, data = 0;
	
	IIC_In_Mode();
	
	for (i = 0; i < 8; i++) {
		IIC_SCL_Out = 0;
		delay_us(2);
		IIC_SCL_Out = 1;	//�ߵ�ƽ�ڼ����ݱ����ȶ�
		
		data <<= 1;
		if (IIC_SDA_In)
			data++;
		
		delay_us(2);
	}
	
	if (0 == ack) {
		IIC_Send_ACK();
	} else {
		IIC_Send_NACK();
	}
	
	return data;
}

/*
* @ brief ��IIC��ȡAT24C02ָ����ַ�ֽڣ�
* @ param ��
*		addr:��Ҫ��ȡ�ֽڵĵ�ַ
* @ return����ȡ�����ֽ�
*/

uint8_t IIC_Read_EEP_Byte(uint16_t addr)
{
	uint8_t data;
	
	IIC_Start_Signal();	//������ʼ�ź�
	IIC_Send_Char(0xA0 + (addr / 256) << 1);	//��ַ����(1010 + A2A1A0 + (R/W))
	IIC_Wait_ACk();	//�ȴ��ӻ�ACK��Ӧ
	IIC_Send_Char(addr % 256);	//���͵͵�ַ
	IIC_Wait_ACk();
	
	IIC_Start_Signal();
	IIC_Send_Char(0xA1);	//�������ģʽ
	IIC_Wait_ACk();
	data = IIC_Receive_Char(1);	//��Ӧ�����
	IIC_Stop_Signal();
	
	return data;
}

/*
* @ brief ��IICд��AT24C02ָ����ַ�ֽڣ�
* @ param ��
*		addr:��Ҫд���ֽڵĵ�ַ��
*		data:д����ֽ�
* @ return��Not
*/

void IIC_Write_EEP_Byte(uint16_t addr, uint8_t data)
{
	IIC_Start_Signal();	//������ʼ�ź�
	IIC_Send_Char(0xA0 + (addr / 256) << 1);	//��ַ����(1010 + A2A1A0 + (R/W))
	IIC_Wait_ACk();	//�ȴ��ӻ�ACK��Ӧ

	IIC_Send_Char(addr % 256);
	IIC_Wait_ACk();
	IIC_Send_Char(data);
	IIC_Wait_ACk();
	IIC_Stop_Signal();
	delay_us(50);
}



