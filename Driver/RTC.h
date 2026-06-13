#ifndef RTC_H
#define RTC_H

#include "HeaderFiles.h"

extern rtc_parameter_struct   rtc_initpara;
extern int16_t rtc_init_flag;

void RTC_Init(void);	// RTC初始化
void rtc_setup(void);	// RTC时钟设置
void rtc_show_time(void);	// RTC时间
void rtc_show_alarm(void);	// RTC闹钟
uint8_t usart_input_threshold(uint32_t value);  // 用作输入值有效校验
void rtc_pre_config(void);
void rtc_setup_user(void);  // 用户设置RTC时间
uint32_t RTC_to_UTC(void);
void UTC_to_RTC(uint32_t timestamp);


#endif




