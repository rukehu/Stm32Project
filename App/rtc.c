#include <stdio.h>
#include "stm32f10x_rtc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_bkp.h"
#include "user.h"
#include "rtc.h"
#include "led.h"


struct date_t{
	uint16_t year;
	uint8_t moon;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t week;
}date;

uint8_t show_flag = 0;
//�·����ݱ�										 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
//ƽ��ÿ�����ڱ�
const uint8_t moon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

int RTC_Init(void)
{
	uint8_t wait = 0;
	//��RTC��Դ���򿪺���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	//�򿪶Ժ���Ĵ����ķ���
	PWR_BackupAccessCmd(ENABLE);
	
	if (0xA0A0 != BKP_ReadBackupRegister(BKP_DR1)){//����Ƿ��ǵ�һ������ʱ��
		//��λ��������
		BKP_DeInit();
		
		//�����ⲿ����������32.768kHz
		RCC_LSEConfig(RCC_LSE_ON);
		while (RESET == RCC_GetFlagStatus(RCC_FLAG_LSERDY) && 250 > wait){//�ȴ�����������ȴ�2500ms
			wait++;
			delay_ms(10);
		}
		if (wait >= 250)
			return -1;	//�����ʼ��ʧ��
		
		//RTCʱ��Դѡ��ΪLSE�ⲿ��������
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
		//ʹ��RTC���ⲿʱ��Դ����
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForLastTask();//�ȴ�RTC�Ĵ������һ��д�������
		RTC_WaitForSynchro();//�ȴ�RTCͬ��
		
		//ʹ��RTC���ж�
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
		
		RTC_EnterConfigMode();//����RTCʱ������ģʽ
		
		RTC_SetPrescaler(32767);	//����RTCԤ��Ƶֵ
		RTC_WaitForLastTask();
		
		RTC_Date_Set(2018, 3, 7, 15, 9, 10);
		RTC_EnterConfigMode();//�˳�RTCʱ������ģʽ
		
		BKP_WriteBackupRegister(BKP_DR1, 0XA0A0);//��ָ���ĺ���Ĵ���д���û���������
	} else {	//��ʾrtc��mcu��λǰ�Ѿ����ù��������������
		RTC_WaitForSynchro();	//�ȴ����һ��д�������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();	//�ȴ����һ��rtc�Ĵ���д�������
	}
	//����rtc NVIC �ж�
	RTC_NVIC_Config();
	
	return 0;
}


int RTC_Date_Set(uint16_t year, uint8_t moon, uint8_t day, 
									uint8_t hour, uint8_t min, uint8_t sec)
{
	uint16_t t;
	uint32_t seccount;
	
	if (year >= 1990 || 2099 >= year) {
		for (t = 1990; t < year; t++) {//��������ݵ������
			if (0 == Leap_Year_Check(t))
				seccount += 31622400;	//�����������
			else
				seccount += 31536000;	//ƽ���������
		}
		moon--;
		for (t = 0; t < moon; t++) {
			seccount += (u32)moon_table[t]*86400;	//�·ݵ�������
			if (0 == Leap_Year_Check(t))
				seccount += 1 * 24*60*60;	//�����һ���������
		}
		//�õ��������е�����
		seccount += (u32)(day-1)*86400;
		seccount += (u32)hour*3600;
		seccount += (u32)min*60;
		seccount += sec;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		
		//ʹ��RTC�󱸼Ĵ�������
		PWR_BackupAccessCmd(ENABLE);
		
		//����RTC�Ĵ�������ֵ
		RTC_SetCounter(seccount);
		RTC_WaitForLastTask(); //�ȴ��Ĵ���д���
		
		return 0;
	}
	
	return -1;
}

int RTC_Date_Get(void)
{
	static uint16_t daycnt = 0;
	uint32_t timecount = 0; 
	uint32_t temp = 0;
	uint16_t temp1 = 0;	  
	
	//��ȡ��ǰ����ֵ
  timecount = RTC_GetCounter();	 
 	temp=timecount/86400;   //�õ�����
	if(daycnt != temp)//���������һ��
	{	  
		daycnt = temp;
		temp1 = 1990;	//��1990�꿪ʼ
		while (temp >= 365)
		{				 
			if(0 == Leap_Year_Check(temp1)) //���������
			{
				if(temp >= 366) {
					temp -= 366; //���������������
				} else {
					temp1++;
					break;
				}  
			} else {
				temp-=365;	  //ƽ��
			}
			temp1++;  
		}   
		date.year = temp1;//��ȡ���
		
		temp1=0;
		while(temp>=28)//������һ����
		{
			if((0 == Leap_Year_Check(date.year)) && (temp1==1))//�Ƿ���������·�
			{
				if(temp >= 29)
					temp -= 29;//�����������
				else 
					break; 
			} else {
				if(temp >= moon_table[temp1])
					temp-=moon_table[temp1]; //ƽ��
				else 
					break;
			}
			temp1++;  
		}
		date.moon = temp1+1;	//�·�
		//date.day = temp+1;  	//����
		date.day = temp;		
	}
	temp = timecount%86400;   //�õ�������   	   
	date.hour=temp/3600;     	//ʱ
	date.min=(temp%3600)/60; 	//��	
	date.sec=(temp%3600)%60; 	//��
	date.week=RTC_Get_Week(date.year, date.moon, date.day); //�õ�week 
	
	return 0;
}

int RTC_Alarm_Set(uint16_t year, uint8_t moon, uint8_t day, 
									uint8_t hour, uint8_t min, uint8_t sec)
{
	uint16_t t;
	uint32_t seccount;
	
	if (year >= 1990 || 2099 >= year) {
		for (t = 1990; t < year; t++) {//��������ݵ������
			if (0 == Leap_Year_Check(t))
				seccount += 31622400;	//�����������
			else
				seccount += 31536000;	//ƽ���������
		}
		moon--;
		for (t = 0; t < moon; t++) {
			seccount += (u32)moon_table[t]*86400;	//�·ݵ�������
			if (0 == Leap_Year_Check(t))
				seccount += 1 * 24*60*60;	//�����һ���������
		}
		//�õ��������е�����
		seccount += (u32)(day-1)*86400;
		seccount += (u32)hour*3600;
		seccount += (u32)min*60;
		seccount += sec;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		
		//ʹ��RTC�󱸼Ĵ�������
		PWR_BackupAccessCmd(ENABLE);
		
		//����RTC���ӼĴ�ֵ
		RTC_SetAlarm(seccount);
		RTC_WaitForLastTask(); //�ȴ��Ĵ���д���
		
		return 0;
	}
	
	return -1;    
}

/*
*����Ƿ�������
*�ǣ�����0�����Ƿ���-1��
*/
int Leap_Year_Check(uint16_t year)
{
	if ((year%4 == 0 && year%100 != 0) || year%400 == 0)
		return 0;
	else
		return -1;
}

/*
*���������ռ�������������ڼ�
*/
uint8_t RTC_Get_Week(uint16_t year, uint8_t moon, uint8_t day)
{
	uint16_t temp;
	uint8_t year_h, year_l;
	
	year_h = year / 100;
	year_l = year % 100;
	if (19 < year_h)	//���Ϊ21���ͣ�����ݼ�100��
		year_l += 100;
	
	//ֻ����1900��֮�������
	temp = year_l + year_l / 4;
	temp %= 7;
	temp = temp + day + table_week[moon-1];
	
	if ((year_l%4 == 0) && (moon < 3))
		temp--;
	
	return (temp % 7);
}

void show_rtc_date(void)
{
	if (0 != show_flag) {
		printf("RTC TIME:%d-%d-%d %d:%d:%d [%d]\n",
					date.year, date.moon, date.day, date.hour, date.min, date.sec, date.week);
		show_flag = 0;
	}
}

void RTC_IRQHandler(void)
{
	led_n(3) = !led_n(3);
	if (RTC_GetITStatus(RTC_IT_SEC) == SET){//���ж�
		RTC_ClearITPendingBit(RTC_IT_SEC);
		if (0 == show_flag) {
			RTC_Date_Get();
			show_flag = 1;
		}
	}
		
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//�����ж�
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);		//��������жϹ���  	
		RTC_Date_Get();  	
  }
	
	//RTC_ClearITPendingBit(RTC_IT_OW);		//�������ж�
	RTC_WaitForLastTask();
}
