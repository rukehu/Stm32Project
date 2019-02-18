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
//月份数据表										 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
//平年每月日期表
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
	//打开RTC电源，打开后备域时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	//打开对后备域寄存器的访问
	PWR_BackupAccessCmd(ENABLE);
	
	if (0xA0A0 != BKP_ReadBackupRegister(BKP_DR1)){//检测是否是第一次配置时钟
		//复位备份区域
		BKP_DeInit();
		
		//开启外部低速振荡器，32.768kHz
		RCC_LSEConfig(RCC_LSE_ON);
		while (RESET == RCC_GetFlagStatus(RCC_FLAG_LSERDY) && 250 > wait){//等待晶振起振，最长等待2500ms
			wait++;
			delay_ms(10);
		}
		if (wait >= 250)
			return -1;	//晶振初始化失败
		
		//RTC时钟源选择为LSE外部低速振荡器
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
		//使能RTC的外部时钟源输入
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForLastTask();//等待RTC寄存器最近一次写操作完成
		RTC_WaitForSynchro();//等待RTC同步
		
		//使能RTC秒中断
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
		
		RTC_EnterConfigMode();//进入RTC时间配置模式
		
		RTC_SetPrescaler(32767);	//设置RTC预分频值
		RTC_WaitForLastTask();
		
		RTC_Date_Set(2018, 3, 7, 15, 9, 10);
		RTC_EnterConfigMode();//退出RTC时间配置模式
		
		BKP_WriteBackupRegister(BKP_DR1, 0XA0A0);//向指定的后备域寄存器写入用户程序数据
	} else {	//表示rtc在mcu复位前已经配置过，继续进入计数
		RTC_WaitForSynchro();	//等待最近一次写操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();	//等待最近一次rtc寄存器写操作完成
	}
	//配置rtc NVIC 中断
	RTC_NVIC_Config();
	
	return 0;
}


int RTC_Date_Set(uint16_t year, uint8_t moon, uint8_t day, 
									uint8_t hour, uint8_t min, uint8_t sec)
{
	uint16_t t;
	uint32_t seccount;
	
	if (year >= 1990 || 2099 >= year) {
		for (t = 1990; t < year; t++) {//将所有年份的秒相加
			if (0 == Leap_Year_Check(t))
				seccount += 31622400;	//闰年的总秒数
			else
				seccount += 31536000;	//平年的总秒数
		}
		moon--;
		for (t = 0; t < moon; t++) {
			seccount += (u32)moon_table[t]*86400;	//月份的秒总数
			if (0 == Leap_Year_Check(t))
				seccount += 1 * 24*60*60;	//闰年多一天的秒总数
		}
		//得到日期所有的秒数
		seccount += (u32)(day-1)*86400;
		seccount += (u32)hour*3600;
		seccount += (u32)min*60;
		seccount += sec;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		
		//使能RTC后备寄存器访问
		PWR_BackupAccessCmd(ENABLE);
		
		//设置RTC寄存器计数值
		RTC_SetCounter(seccount);
		RTC_WaitForLastTask(); //等待寄存器写完毕
		
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
	
	//获取当前计数值
  timecount = RTC_GetCounter();	 
 	temp=timecount/86400;   //得到天数
	if(daycnt != temp)//如果超过了一天
	{	  
		daycnt = temp;
		temp1 = 1990;	//从1990年开始
		while (temp >= 365)
		{				 
			if(0 == Leap_Year_Check(temp1)) //如果是闰年
			{
				if(temp >= 366) {
					temp -= 366; //计算闰年的秒钟数
				} else {
					temp1++;
					break;
				}  
			} else {
				temp-=365;	  //平年
			}
			temp1++;  
		}   
		date.year = temp1;//获取年份
		
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if((0 == Leap_Year_Check(date.year)) && (temp1==1))//是否是闰年二月份
			{
				if(temp >= 29)
					temp -= 29;//闰年的秒钟数
				else 
					break; 
			} else {
				if(temp >= moon_table[temp1])
					temp-=moon_table[temp1]; //平年
				else 
					break;
			}
			temp1++;  
		}
		date.moon = temp1+1;	//月份
		//date.day = temp+1;  	//日期
		date.day = temp;		
	}
	temp = timecount%86400;   //得到秒钟数   	   
	date.hour=temp/3600;     	//时
	date.min=(temp%3600)/60; 	//分	
	date.sec=(temp%3600)%60; 	//秒
	date.week=RTC_Get_Week(date.year, date.moon, date.day); //得到week 
	
	return 0;
}

int RTC_Alarm_Set(uint16_t year, uint8_t moon, uint8_t day, 
									uint8_t hour, uint8_t min, uint8_t sec)
{
	uint16_t t;
	uint32_t seccount;
	
	if (year >= 1990 || 2099 >= year) {
		for (t = 1990; t < year; t++) {//将所有年份的秒相加
			if (0 == Leap_Year_Check(t))
				seccount += 31622400;	//闰年的总秒数
			else
				seccount += 31536000;	//平年的总秒数
		}
		moon--;
		for (t = 0; t < moon; t++) {
			seccount += (u32)moon_table[t]*86400;	//月份的秒总数
			if (0 == Leap_Year_Check(t))
				seccount += 1 * 24*60*60;	//闰年多一天的秒总数
		}
		//得到日期所有的秒数
		seccount += (u32)(day-1)*86400;
		seccount += (u32)hour*3600;
		seccount += (u32)min*60;
		seccount += sec;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		
		//使能RTC后备寄存器访问
		PWR_BackupAccessCmd(ENABLE);
		
		//设置RTC闹钟寄存值
		RTC_SetAlarm(seccount);
		RTC_WaitForLastTask(); //等待寄存器写完毕
		
		return 0;
	}
	
	return -1;    
}

/*
*检测是否是闰年
*是：返回0，不是返回-1；
*/
int Leap_Year_Check(uint16_t year)
{
	if ((year%4 == 0 && year%100 != 0) || year%400 == 0)
		return 0;
	else
		return -1;
}

/*
*根据年月日计算该日期是星期几
*/
uint8_t RTC_Get_Week(uint16_t year, uint8_t moon, uint8_t day)
{
	uint16_t temp;
	uint8_t year_h, year_l;
	
	year_h = year / 100;
	year_l = year % 100;
	if (19 < year_h)	//如果为21世纪，则年份加100年
		year_l += 100;
	
	//只计算1900年之后的闰年
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
	if (RTC_GetITStatus(RTC_IT_SEC) == SET){//秒中断
		RTC_ClearITPendingBit(RTC_IT_SEC);
		if (0 == show_flag) {
			RTC_Date_Get();
			show_flag = 1;
		}
	}
		
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//闹钟中断
	{
		RTC_ClearITPendingBit(RTC_IT_ALR);		//清除闹钟中断挂起  	
		RTC_Date_Get();  	
  }
	
	//RTC_ClearITPendingBit(RTC_IT_OW);		//清除溢出中断
	RTC_WaitForLastTask();
}
