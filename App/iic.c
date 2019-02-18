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
* @ before：IIC 配置输出模式；
* @ param ：Not
* @ return：Not
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
* @ before：IIC 配置输入模式；
* @ param ：Not
* @ return：Not
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
* @ before：IIC 发送起始信号；
* @ param ：Not
* @ return：Not
*/
void IIC_Start_Signal(void)
{
	IIC_Out_Mode();
	
	IIC_SDA_Out = 1;
	IIC_SCL_Out = 1;
	delay_us(5);
	IIC_SDA_Out = 0;	//在SCL为高时，SDA由高到底，发送起始信号
	delay_us(6);
	IIC_SCL_Out = 0;	//将SCL拉低，准备发送或接收数据
	
}

/*
* @ brief ：IIC 发送停止信号；
* @ param ：Not
* @ return：Not
*/
void IIC_Stop_Signal(void)
{
	IIC_Out_Mode();
	
	IIC_SCL_Out = 0;
	IIC_SDA_Out = 0;
	IIC_SCL_Out = 1;	//SCL保存高电平
	delay_us(6);
	IIC_SDA_Out = 1;	//SDA由低到高,产生终止信号
	delay_us(5);
}

/*
* @ brief ：IIC 等待从机ACK响应；
* @ param ：Not
* @ return：0从机响应，-1从机未响应
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
			IIC_Stop_Signal();	//从机未响应，终止此次数据传输
			return -1;
		}
	}
	IIC_SCL_Out = 0;	//SCL由高到底，完成一次应答时钟脉冲
	
	return 0;
}

/*
* @ brief ：IIC从机模式下，发送应答信号；
* @ param ：Not
* @ return：Not
*/
void IIC_Send_ACK(void)
{
	IIC_SCL_Out = 0;
	IIC_Out_Mode();
	IIC_SDA_Out = 0;	//应答为低电平
	delay_us(2);
	IIC_SCL_Out = 1;
	delay_us(5);
	IIC_SCL_Out = 0;	//IIC由高到第，完成一次应答时钟脉冲	
}

/*
* @ brief ：IIC从机模式下，发送非应答信号；
* @ param ：Not
* @ return：Not
*/
void IIC_Send_NACK(void)
{
	IIC_SCL_Out = 0;
	IIC_Out_Mode();
	IIC_SDA_Out = 1;	//应答为高电平
	delay_us(2);
	IIC_SCL_Out = 1;
	delay_us(5);
	IIC_SCL_Out = 0;	//IIC由高到第，完成一次应答时钟脉冲	
}

/*
* @ brief ：IIC发送一个字符；
* @ param ：
*		data:发送的字符
* @ return：Not
*/
void IIC_Send_Char(uint8_t data)
{
	uint8_t i;
	
	IIC_Out_Mode();
	IIC_SCL_Out = 0;	//准备开始传输数据,低电平交换数据
	
	for (i = 0; i < 8; i++) {
		if (data & 0x80) {
			IIC_SDA_Out = 1;
		} else {
			IIC_SDA_Out = 0;
		}
		data <<= 1;
		
		delay_us(2);
		IIC_SCL_Out = 1;	//高电平数据保持稳定
		delay_us(2);
		IIC_SCL_Out = 0;	//下降沿采样数据
		delay_us(2);
	}
}

/*
* @ brief ：IIC读取一个字符；
* @ param ：
*		ack:读取一个字符后发送的应答情况；0（发送应答），1（不发送应答，用于最后一个字节数据传输完毕）；
* @ return：接收到的字符
*/
uint8_t IIC_Receive_Char(uint8_t ack)
{
	uint8_t i, data = 0;
	
	IIC_In_Mode();
	
	for (i = 0; i < 8; i++) {
		IIC_SCL_Out = 0;
		delay_us(2);
		IIC_SCL_Out = 1;	//高电平期间数据保持稳定
		
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
* @ brief ：IIC读取AT24C02指定地址字节；
* @ param ：
*		addr:需要读取字节的地址
* @ return：读取到的字节
*/

uint8_t IIC_Read_EEP_Byte(uint16_t addr)
{
	uint8_t data;
	
	IIC_Start_Signal();	//发送起始信号
	IIC_Send_Char(0xA0 + (addr / 256) << 1);	//地址类型(1010 + A2A1A0 + (R/W))
	IIC_Wait_ACk();	//等待从机ACK响应
	IIC_Send_Char(addr % 256);	//发送低地址
	IIC_Wait_ACk();
	
	IIC_Start_Signal();
	IIC_Send_Char(0xA1);	//进入接收模式
	IIC_Wait_ACk();
	data = IIC_Receive_Char(1);	//非应答接收
	IIC_Stop_Signal();
	
	return data;
}

/*
* @ brief ：IIC写入AT24C02指定地址字节；
* @ param ：
*		addr:需要写入字节的地址；
*		data:写入的字节
* @ return：Not
*/

void IIC_Write_EEP_Byte(uint16_t addr, uint8_t data)
{
	IIC_Start_Signal();	//发送起始信号
	IIC_Send_Char(0xA0 + (addr / 256) << 1);	//地址类型(1010 + A2A1A0 + (R/W))
	IIC_Wait_ACk();	//等待从机ACK响应

	IIC_Send_Char(addr % 256);
	IIC_Wait_ACk();
	IIC_Send_Char(data);
	IIC_Wait_ACk();
	IIC_Stop_Signal();
	delay_us(50);
}



