#ifndef MAIN_H
#define MAIN_H

#include "Sampling.h"
#include "HeaderFiles.h"
#include "Flash.h"
#include "parsing.h"
#include "OLED.h"
#include "RTC.h"
#include "DAC.h"

#define PARAM_PAGE       0x08010000U  

//告警记录
typedef struct
{
    rtc_parameter_struct time;      //时间
    uint8_t channel_id;             //通道
    float sampled_value;            //记录当时的采样值
}Alarm_record;

//存储参数结构体
typedef struct
{
    //设备ID
    uint16_t DeviceID;
    //固件版本
    uint8_t Ver1;
    uint8_t Ver2;
    uint8_t Ver3;
    uint8_t Ver4;
    //波特率
    uint32_t Baud_rate;
    //ch0~ch1比率和阈值
    Sampling_channel ch0;
    Sampling_channel ch1;
    //告警记录
    Alarm_record record[10];
}Parameter;

typedef enum {
    APP_STATE_IDLE,           // 空闲，可接收所有命令
    APP_STATE_AUTO_SAMPLING,  // 自动上报中，只响应停止命令
    APP_STATE_SLEEP,          // 睡眠模式（低功耗）
    APP_STATE_WAIT_REBOOT,    // 已回复重启指令，等待串口发送完成复位
    APP_STATE_BOOTLOADER,       // 进入bootloader 
} AppState;


extern AppState appState;
extern Parameter parameter;
// extern uint16_t Device_ID;
extern int flag;
extern int time_interval;
//自动上报时间允许标志
extern int Upload_logo;

void nvic_config(void);
void Save_Parameter(void);
void Read_Parameter(void);
void init_main(void);

#endif


