#ifndef FUNCTION_H
#define FUNCTION_H

#include "gd32f4xx.h"

// 变比变量（静态，仅在本文件使用）
extern float g_ch0_ratio;
extern float g_ch1_ratio;

// ch0-ch1阈值参数
extern float ch0_threshold;
extern float ch1_threshold;

// ==================== CH0 函数声明（滑动变阻器） ====================
uint16_t ADC_Read_CH0(void);              // 读取CH0原始值
float ADC_Get_CH0_Voltage(void);          // 获取CH0电压
float ADC_Get_CH0_Value(void);            // 获取CH0实际值（乘变比后）
void ADC_Set_CH0_Ratio(float ratio);      // 设置CH0变比
float ADC_Get_CH0_Ratio(void);            // 获取CH0变比

// ==================== CH1 函数声明（DAC回读） ====================
uint16_t ADC_Read_CH1(void);              // 读取CH1原始值
float ADC_Get_CH1_Voltage(void);          // 获取CH1电压
float ADC_Get_CH1_Value(void);            // 获取CH1实际值（乘变比后）
void ADC_Set_CH1_Ratio(float ratio);      // 设置CH1变比
float ADC_Get_CH1_Ratio(void);            // 获取CH1变比



// ==================== 工具函数 ====================
void Float_To_Bytes_BigEndian(float value, uint8_t *bytes);
float Bytes_To_Float_BigEndian(uint8_t *bytes);
void execute_Command_word(uint16_t Command_word);

#endif
