#ifndef __RTC_H__
#define __RTC_H__


int RTC_Init(void);
int Leap_Year_Check(uint16_t year);
int RTC_Date_Set(uint16_t year, uint8_t moon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t RTC_Get_Week(uint16_t year, uint8_t moon, uint8_t day);
int RTC_Alarm_Set(uint16_t year, uint8_t moon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
void show_rtc_date(void);
int RTC_Date_Get(void);
#endif /*__RTC_H__*/
