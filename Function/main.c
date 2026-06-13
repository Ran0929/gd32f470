#include "HeaderFiles.h"
#include "main.h"
#include "parsing.h"
#include "OLED.h"
#include "RTC.h"
#include "DAC.h"
//需要存储到FLASH的参数
Parameter parameter;
uint8_t reboot=0;
//固件版本,已写入flash，无需变量
//02.00.01.00
int firmware_version1=0x02;
int firmware_version2=0x00;
int firmware_version3=0x01;
int firmware_version4=0x00;
//自动上报标志
int flag=0;
//自动上报时间间隔
int time_interval=1000;
//自动上报时间允许标志
int Upload_logo=0;
int main()
{
    nvic_config();
    systick_config();   // 时钟嘀嗒定时器配置1ms
    // parameter.Baud_rate = 19200;
    // parameter.ch0_rate = 1.0;
    // parameter.ch1_rate = 1.0;
    // parameter.ch0_threshold = 3.3;
    // parameter.ch1_threshold = 3.3;
    // Save_Parameter();
    // delay_1ms(10);
    Read_Parameter();   //读falsh数据
    //从FLASH读波特率赋值
    SET_Baud_rate=parameter.Baud_rate;
    g_ch0_ratio=parameter.ch0_rate;
    g_ch1_ratio=parameter.ch1_rate;
    ch0_threshold=parameter.ch0_threshold;
    ch1_threshold=parameter.ch1_threshold;

    USART0_Config();    //串口0初始化

    // printf("波特率：%d\r\n",parameter.Baud_rate);
    // printf("g_ch0_ratio=%0.2f\r\n",g_ch0_ratio);
    // printf("g_ch1_ratio=%0.2f\r\n",g_ch1_ratio);
    // printf("ch0_threshold=%0.2f\r\n",ch0_threshold);
    // printf("ch1_threshold=%0.2f\r\n",ch1_threshold);

    RTC_Init();         //RTC实时时钟初始化
    LED_init();         //LED初始化
    OLED_Init();		// OLED初始化
    ADC_port_init();    //ADC初始化
    DAC_Init();          //DAC初始化

    response_value.Start_marker=0xA5B6;
    response_value.Device_ID=parameter.DeviceID;
    response_value.Frame_type=0x05;
    response_value.Command_word=0x8888;
    response_value.Message_length=0x00;
    response_value.Protocol_version=0x02;
    response_value.Content[0]=0x00;
    Frame_assembly();
    
    while(1)
    {
        isOLED();
        //获取帧解析状态
        int status=Frame_analysis();
        //帧完成解析时
        if(status==finish)
        {
            //printf("head=%d tail=%d\r\n",rx_head,rx_tail);
            //将帧状态重置为NONE，等待下次解析
            parsingStatus=R_NONE;
            //开始处理帧指令
            execute_Command_word(receive_value.Command_word);
            //等待发送应答
            response_status=WAIT;

            //清空接收缓冲区
            for(int i=0;i<RX_BUF_SIZE;i++)
            rx_buf[i]=0;
        }
        //等待答复
        if(response_status==WAIT)
        {
            
            //组帧
            Frame_assembly();
            //命令字等于0111时，查询设备ID
            // if(receive_value.Command_word==0x0111)
            // {
            //     char test[]={'A','5','B','6','0','0','0','1','0','2','0','1','1','1','0','2','0','2','0','0','0','2','4','5','C','C','B','6','A','5'};
            //     for(int i=0;i<30;i++)
            //     printf("%c",test[i]);
            //     printf("\r\n");

                //清空接收缓冲区
                for(int i=0;i<RX_BUF_SIZE;i++)
                rx_cmd[i]=0;

                //初始化帧结构体
                deinit_GFF();
                response_status=T_NONE;
            // }
        }
        if(flag==1)
        {
            if(Upload_logo==1)
            {
                Upload_logo=0;
                //设备自动上报中
                response_value.Start_marker = 0xA5B6;
                response_value.Device_ID = parameter.DeviceID;
                response_value.Frame_type = 0x02;       // 应答帧
                response_value.Command_word = 0x0302;
                response_value.Message_length = 0x12;   // 1字节数据
                response_value.Protocol_version = 0x02;

                rtc_current_time_get(&rtc_initpara);
                uint32_t timestamp = RTC_to_UTC();
                response_value.Content[0]=timestamp>>24;
                response_value.Content[1]=timestamp>>16;
                response_value.Content[2]=timestamp>>8;
                response_value.Content[3]=timestamp;
                float result_0=ADC_Get_CH0_Voltage()*g_ch0_ratio;  //读取滑动变阻器的电压值
                Float_To_Bytes_BigEndian(result_0, response_value.Content+4);
                float result_1=ADC_Get_CH1_Voltage()*g_ch1_ratio;  //读取滑动变阻器的电压值
                Float_To_Bytes_BigEndian(result_1, response_value.Content+8);
                Frame_assembly();
            }
        }
        //设备等待重启
        if(reboot==1)
        {
            reboot=0;
            NVIC_SystemReset();// 系统复位
        }
    }
}

void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);// 设置中断优先级分组
//    nvic_irq_enable(EXTI3_IRQn,2,0);					// 使能EXTI3中断key，优先级为2
}

void Save_Parameter(void)
{
    fmc_unlock(); // 解锁

    // 擦除页面每页4KB
    fmc_page_erase(PARAM_PAGE);

    // 写入数据
    fmc_halfword_program(PARAM_FLASH_ADDR,parameter.DeviceID);

    fmc_byte_program(PARAM_FLASH_ADDR+2,parameter.Ver1);
    fmc_byte_program(PARAM_FLASH_ADDR+3,parameter.Ver2);
    fmc_byte_program(PARAM_FLASH_ADDR+4, parameter.Ver3);
    fmc_byte_program(PARAM_FLASH_ADDR+5,parameter.Ver4);

    //波特率写入
    fmc_word_program(PARAM_FLASH_ADDR+8,parameter.Baud_rate);

    //ch0-ch1变比和阈值
    fmc_word_program(PARAM_FLASH_ADDR+12, *(uint32_t*)&parameter.ch0_rate);
    fmc_word_program(PARAM_FLASH_ADDR+16, *(uint32_t*)&parameter.ch1_rate);
    fmc_word_program(PARAM_FLASH_ADDR+20, *(uint32_t*)&parameter.ch0_threshold);
    fmc_word_program(PARAM_FLASH_ADDR+24, *(uint32_t*)&parameter.ch1_threshold);

    fmc_lock();   // 上锁
}

void Read_Parameter(void)
{
    parameter.DeviceID = *(volatile uint16_t *)(PARAM_FLASH_ADDR);

    parameter.Ver1 = *(volatile uint8_t *)(PARAM_FLASH_ADDR+2);

    parameter.Ver2 = *(volatile uint8_t *)(PARAM_FLASH_ADDR+3);

    parameter.Ver3 = *(volatile uint8_t *)(PARAM_FLASH_ADDR+4);

    parameter.Ver4 = *(volatile uint8_t *)(PARAM_FLASH_ADDR+5);

    parameter.Baud_rate = *(volatile uint32_t *)(PARAM_FLASH_ADDR+8);

    parameter.ch0_rate =  *(volatile float *)(PARAM_FLASH_ADDR+12);

    parameter.ch1_rate =  *(volatile float *)(PARAM_FLASH_ADDR+16);

    parameter.ch0_threshold =  *(volatile float *)(PARAM_FLASH_ADDR+20);

    parameter.ch1_threshold =  *(volatile float *)(PARAM_FLASH_ADDR+24);
    
}

