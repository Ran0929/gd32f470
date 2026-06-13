#include "DAC.h"
#include "systick.h"
#include "HeaderFiles.h"
// DAC初始化
void DAC_Init(void)
{
    // 1. 使能时钟
    rcu_periph_clock_enable(RCU_GPIOA);   // 使能GPIOA时钟（PA4所在端口）
    rcu_periph_clock_enable(RCU_DAC);     // 使能DAC模块时钟
    
    // 2. 配置PA4引脚为模拟功能
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
    
    // 3. 复位DAC
    dac_deinit(DAC0);
    
    // 4. 配置DAC参数
    dac_trigger_disable(DAC0, DAC_OUT0);                    // 禁用外部触发（使用软件触发）
    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_DISABLE); // 禁用波形生成
    dac_output_buffer_disable(DAC0, DAC_OUT0);              // 禁用输出缓冲
    
    // 5. 使能DAC
    dac_enable(DAC0, DAC_OUT0);
    
    // // 6. 设置初始输出为0V
    // DAC_Set_Output(1024);


    // 设置 DAC 输出 0.82V
    DAC_Set_Output(2049);
    delay_1ms(10);
    
}

// 设置DAC输出值（原始值 0-4095）
void DAC_Set_Output(uint16_t value)
{
    // 限制输入范围
    if(value > 4095) value = 4095;
    
    // 设置DAC输出（12位右对齐）
    dac_data_set(DAC0, DAC_OUT0, DAC_ALIGN_12B_R, value);
    
    // 等待DAC输出稳定（重要！）
    delay_1ms(5);
}

// 设置DAC输出电压（伏特 0-3.0V）
void DAC_Set_Voltage(float voltage)
{
    uint16_t dac_value;
    
    // 电压转DAC值
    if(voltage <= 0) {
        dac_value = 0;
    } else if(voltage >= 3.0f) {
        dac_value = 4095;
    } else {
        dac_value = (uint16_t)((voltage / 3.0f) * 4095.0f);
    }
    
    // 设置输出
    DAC_Set_Output(dac_value);
}



