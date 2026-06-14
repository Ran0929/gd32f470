#include "main.h"

//需要存储到FLASH的参数
Parameter parameter;
//状态机
AppState appState=APP_STATE_IDLE;
//自动上报标志
int flag=0;
//自动上报时间间隔
int time_interval=1000;
//自动上报时间允许标志
int Upload_logo=0;
int main()
{
    // pmu_backup_write_enable();

    // /* 复位 Backup Domain */
    // rcu_bkp_reset_enable();
    // rcu_bkp_reset_disable();

    init_main();        //初始化采样通道等结构体
    nvic_config();      //中断号配置
    systick_config();   // 时钟嘀嗒定时器配置1ms
    Read_Parameter();   //读falsh数据
    //从FLASH读波特率赋值
    SET_Baud_rate=parameter.Baud_rate;
    USART0_Config();    //串口0初始化
    RTC_Init();         //RTC实时时钟初始化
    LED_init();         //LED初始化
    OLED_Init();		// OLED初始化
    ADC_port_init();    //ADC初始化
    DAC_Init();          //DAC初始化

    //发送心跳帧
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
        //显示OLED
        isOLED();
        //获取帧解析状态
        int status=Frame_analysis();
        //帧完成解析时
        if(status==finish)
        {
            //将帧状态重置为NONE，等待下次解析
            parsingStatus=R_NONE;
            //开始处理帧指令
            execute_Command_word(receive_value.Command_word);
            //初始化应答结构体
            deinit_GFF();
            if(response_status!=T_NONE)
            {
                //等待发送应答
                response_status=WAIT;
            }
            //清空接收缓冲区
            for(int i=0;i<RX_BUF_SIZE;i++)
            rx_buf[i]=0;
        }
        //等待答复
        if(response_status==WAIT)
        {
            //组帧
            Frame_assembly();
            //清空接收缓冲区
            for(int i=0;i<RX_BUF_SIZE;i++)
            rx_cmd[i]=0;
            //初始化帧结构体
            deinit_GFF();
            response_status=T_NONE;
        }
        if(appState==APP_STATE_AUTO_SAMPLING)//自动采样上报中
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
                float result_0=ADC_Get_CH0_Voltage()*parameter.ch0.rate;  //读取滑动变阻器的电压值
                Float_To_Bytes_BigEndian(result_0, response_value.Content+4);
                float result_1=ADC_Get_CH1_Voltage()*parameter.ch1.rate;  //读取滑动变阻器的电压值
                Float_To_Bytes_BigEndian(result_1, response_value.Content+8);
                Frame_assembly();
            }
        }
        //设备等待重启
        if(appState==APP_STATE_WAIT_REBOOT)
        {
            appState=APP_STATE_IDLE;
            NVIC_SystemReset();// 系统复位
        }
    }
}

void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);// 设置中断优先级分组
//    nvic_irq_enable(EXTI3_IRQn,2,0);					// 使能EXTI3中断key，优先级为2
}

void init_main(void)
{
    init_Sampling_channel(&ch0);
    init_Sampling_channel(&ch1);
}