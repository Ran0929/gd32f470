#ifndef DAC_H
#define DAC_H

#include "gd32f4xx.h"

// 函数声明
void DAC_Init(void);
void DAC_Set_Output(uint16_t value);
void DAC_Set_Voltage(float voltage);

#endif
