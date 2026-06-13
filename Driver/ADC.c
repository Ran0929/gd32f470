// adc.c
#include "ADC.h"
#include "HeaderFiles.h"
#include "systick.h"


// 全局变量
ADC_WorkMode_t g_adc_mode = ADC_MODE_SINGLE;
uint16_t g_last_adc_value = 0;

// ADC引脚配置
#define ADC_GPIO_PORT      GPIOC
#define ADC_GPIO_PIN       GPIO_PIN_0
#define ADC_CHANNEL        ADC_CHANNEL_10

void ADC_port_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC0);
    
    gpio_mode_set(ADC_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_GPIO_PIN);
    
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    
    ADC_Init();
}

void ADC_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC0);
    
    // 配置ADC引脚（模拟模式）
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);  // PC0 - CH0（电位器）
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);  // PC1 - CH1（DAC回读）
    
    adc_clock_config(ADC_ADCCK_PCLK2_DIV8);
    adc_deinit();
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);
    
    // 配置ADC通道（默认CH0）
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_56);
    
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
    
    adc_enable(ADC0);
    
    delay_1ms(1);
    
    adc_calibration_enable(ADC0);
}

void ADC_SetWorkMode(ADC_WorkMode_t mode)
{
    if (g_adc_mode == mode)
        return;
    
    g_adc_mode = mode;
    
    if (mode == ADC_MODE_SINGLE) {
        adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
        adc_enable(ADC0);
        delay_1ms(1);
    } else {
        adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
        adc_enable(ADC0);
        delay_1ms(1);
    }
}

void ADC_StartContinuous(void)
{
    if (g_adc_mode == ADC_MODE_CONTINUOUS) {
        adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
    }
}

void ADC_StopContinuous(void)
{
    if (g_adc_mode == ADC_MODE_CONTINUOUS) {
        adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
        delay_1ms(1);
        adc_enable(ADC0);
        delay_1ms(1);
    }
}

uint16_t ADC_ReadValue(void)
{
    uint16_t adc_value;
    
    if (g_adc_mode == ADC_MODE_SINGLE) {
        adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
        while (adc_flag_get(ADC0, ADC_FLAG_EOC) == RESET);
        adc_value = adc_routine_data_read(ADC0);
        g_last_adc_value = adc_value;
        return adc_value;
    } else {
        if (adc_flag_get(ADC0, ADC_FLAG_EOC) == SET) {
            g_last_adc_value = adc_routine_data_read(ADC0);
        }
        return g_last_adc_value;
    }
}






