#ifndef ADC_H
#define ADC_H

#include "gd32f4xx.h"

// 先定义类型
typedef enum {
    ADC_MODE_SINGLE = 0,
    ADC_MODE_CONTINUOUS = 1
} ADC_WorkMode_t;

// 再声明外部变量（现在类型已定义）
extern ADC_WorkMode_t g_adc_mode;
extern uint16_t g_last_adc_value;

// 函数声明
void ADC_port_init(void);
void ADC_Init(void);
void ADC_SetWorkMode(ADC_WorkMode_t mode);
void ADC_StartContinuous(void);
void ADC_StopContinuous(void);
uint16_t ADC_ReadValue(void);
float POT_Read_Voltage(void);

#endif


