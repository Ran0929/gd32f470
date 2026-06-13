#include "LED.h"

void LED_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);    // 初始化GPIO_A总线时钟
    
    //系统状态指示灯
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_7);             // GPIO模式设置为输出
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);   // 输出参数设置
    gpio_bit_reset(GPIOA, GPIO_PIN_7);                                              // 引脚初始电平为低电平
    
    //采集工作指示灯
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_5);             // GPIO模式设置为输出
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);   // 输出参数设置
    gpio_bit_reset(GPIOA, GPIO_PIN_5);                                              // 引脚初始电平为低电平
}

void LED_overturn(void)
{
    static int state=0;
    if(state==0)
    {
        gpio_bit_reset(GPIOA, GPIO_PIN_7);
        state=1;
    }
    else
    {
        gpio_bit_set(GPIOA, GPIO_PIN_7);
        state=0;
    }
}

