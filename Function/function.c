#include "ADC.h"
#include "DAC.h"
#include "HeaderFiles.h"
#include "function.h"
#include "parsing.h"
#include "RTC.h"


// 变比变量（静态，仅在本文件使用）
float g_ch0_ratio = 2;
float g_ch1_ratio = 2;

// ch0-ch1阈值参数
float ch0_threshold = 21.59;
float ch1_threshold = 21.59;

// ==================== CH0 读取函数（滑动变阻器） ====================

// 读取CH0（电位器）原始值
uint16_t ADC_Read_CH0(void)
{
    uint16_t adc_value;
    
    // 配置CH0通道（PC0 = ADC通道10）
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_56);
    
    // 软件触发转换
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
    
    // 等待转换完成
    while(adc_flag_get(ADC0, ADC_FLAG_EOC) == RESET);
    
    // 读取转换值
    adc_value = adc_routine_data_read(ADC0);
    
    return adc_value;
}

// 读取CH0电压（电位器）- 参考电压3.0V
float ADC_Get_CH0_Voltage(void)
{
    uint16_t adc_raw = ADC_Read_CH0();
    return 1.0f + (adc_raw / 4095.0f) * 2.0f;
}

// 获取CH0实际值（乘变比后）- 用于上报
float ADC_Get_CH0_Value(void)
{
    return ADC_Get_CH0_Voltage() * g_ch0_ratio;
}

// 设置CH0变比
void ADC_Set_CH0_Ratio(float ratio)
{
    if(ratio > 0 && ratio < 100) {
        g_ch0_ratio = ratio;
    }
}

// 获取CH0变比
float ADC_Get_CH0_Ratio(void)
{
    return g_ch0_ratio;
}

// ==================== CH1 读取函数（DAC回读） ====================

// 读取CH1（DAC回读）原始值
uint16_t ADC_Read_CH1(void)
{

    uint16_t adc_value;
    
    // 配置CH1通道（PC3 = ADC通道13）
    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_13, ADC_SAMPLETIME_56);
    
    // 软件触发转换
    adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL);
    
    // 等待转换完成
    while(adc_flag_get(ADC0, ADC_FLAG_EOC) == RESET);
    
    // 读取转换值
    adc_value = adc_routine_data_read(ADC0);
    
    return adc_value;
}

// 获取CH1电压（DAC回读）- 参考电压3.3V
float ADC_Get_CH1_Voltage(void)
{
    uint16_t adc_raw = ADC_Read_CH1();
    return (adc_raw / 4095.0f) * 3.3f;
}

// 获取CH1实际值（乘变比后）- 用于上报
float ADC_Get_CH1_Value(void)
{
    return ADC_Get_CH1_Voltage() * g_ch1_ratio;
}

// 设置CH1变比
void ADC_Set_CH1_Ratio(float ratio)
{
    if(ratio > 0 && ratio < 100) {
        g_ch1_ratio = ratio;
    }
}

// 获取CH1变比
float ADC_Get_CH1_Ratio(void)
{
    return g_ch1_ratio;
}


// ==================== 工具函数 ====================

// 将浮点数转换为大端序字节数组
void Float_To_Bytes_BigEndian(float value, uint8_t *bytes)
{
    uint8_t *p = (uint8_t*)&value;
    bytes[0] = p[3];
    bytes[1] = p[2];
    bytes[2] = p[1];
    bytes[3] = p[0];
}

// 将大端序字节数组转换为浮点数
float Bytes_To_Float_BigEndian(uint8_t *bytes)
{
    float value;
    uint8_t *p = (uint8_t*)&value;
    
    // 大端序转小端序（MCU是小端存储）
    p[0] = bytes[3];
    p[1] = bytes[2];
    p[2] = bytes[1];
    p[3] = bytes[0];
    
    return value;
}

//执行帧命令
void execute_Command_word(uint16_t Command_word)
{
    //初始化应答结构体
    deinit_GFF();

    if(Command_word==0x0101)//设备重启
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0101;
        response_value.Message_length=0x01;
        response_value.Protocol_version=0x02;
        response_value.Content[0]=0xFF;

        //设备重启标志
        appState=APP_STATE_WAIT_REBOOT;
    }
    else if(Command_word==0x0104)//查询固件版本
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0104;
        response_value.Message_length=0x04;
        response_value.Protocol_version=0x02;
        response_value.Content[0]=parameter.Ver1;
        response_value.Content[1]=parameter.Ver2;
        response_value.Content[2]=parameter.Ver3;
        response_value.Content[3]=parameter.Ver4;
    }
    else if (Command_word==0x0105)//设置设备时间
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0105;
        response_value.Message_length=0x01;
        response_value.Protocol_version=0x02;
        response_value.Content[0]=0xFF;
        uint32_t timestamp1 = ((uint32_t)receive_value.Content[0]<<24)|((uint32_t)receive_value.Content[1]<<16)|((uint32_t)receive_value.Content[2]<<8)|((uint32_t)receive_value.Content[3]);
        //转换并写入结构体
        UTC_to_RTC(timestamp1);
        //结构体写入硬件
        rtc_init(&rtc_initpara);
    }
    else if (Command_word==0x0106)//查询设备时间
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0106;
        response_value.Message_length=0x04;
        response_value.Protocol_version=0x02;
        rtc_current_time_get(&rtc_initpara);
        uint32_t timestamp = RTC_to_UTC();
        response_value.Content[0]=timestamp>>24;
        response_value.Content[1]=timestamp>>16;
        response_value.Content[2]=timestamp>>8;
        response_value.Content[3]=timestamp;
    }
    else if(Command_word==0x01A1)//设置设备id
    {
        parameter.DeviceID=((uint16_t)receive_value.Content[0]<<8)|(uint16_t)receive_value.Content[1];

        response_value.Start_marker=0xA5B6;
        response_value.Command_word=0x01A1;
        response_value.Protocol_version=0x02;
        response_value.Content[0]=0xFF;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Message_length=0x01;
        // printf("Content[0]:%04X",receive_value.Content[0]);
        // printf("Content[1]:%04X",receive_value.Content[1]);
        // printf("Device_ID:%04X",parameter.DeviceID);
        Save_Parameter();
    }
    else if (Command_word==0x01A2)//设置波特率
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x01A2;
        response_value.Message_length=0x01;
        response_value.Protocol_version=0x02;
        response_value.Content[0]=0xFF;
        // printf("Content:%02X",receive_value.Content[0]);
        if(receive_value.Content[0]==0x11)
        {
            parameter.Baud_rate=4800;
        }
        else if (receive_value.Content[0]==0x12)
        {
            parameter.Baud_rate=9600;
        }
        else if (receive_value.Content[0]==0x13)
        {
            parameter.Baud_rate=19200;
        }
        else if (receive_value.Content[0]==0x14)
        {
            parameter.Baud_rate=115200;
        }
        Save_Parameter();
        //设备重启标志
        appState=APP_STATE_WAIT_REBOOT;
        
    }
    else if(Command_word==0x0111)//查询ID
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0111;
        response_value.Message_length=0x02;
        response_value.Protocol_version=0x02;

        //回复2字节ID
        response_value.Content[0]=parameter.DeviceID>>8;
        response_value.Content[1]=parameter.DeviceID;
    }
    else if(Command_word==0x0112)//查询波特率
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0112;
        response_value.Message_length=0x01;
        response_value.Protocol_version=0x02;
        if(SET_Baud_rate==115200)
        {
            response_value.Content[0]=0x14;
        }
        else if (SET_Baud_rate==19200)
        {
            response_value.Content[0]=0x13;
        }
        else if (SET_Baud_rate==9600)
        {
            response_value.Content[0]=0x12;
        }
        else if (SET_Baud_rate==4800)
        {
            response_value.Content[0]=0x11;
        }
    }
    else if(Command_word==0x0201)//查询CH0数据(ADC通道0:滑动变阻器)
    {
        float result_0=ADC_Get_CH0_Voltage()*g_ch0_ratio;  //读取滑动变阻器的电压值
        // printf("reslt_0=%0.2f\r\n",result_0);
        
        // printf("g_ch0_ratio=%0.2f\r\n",g_ch0_ratio);
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0201;
        response_value.Message_length=0x04;
        response_value.Protocol_version=0x02;
        
        Float_To_Bytes_BigEndian(result_0, response_value.Content);
    }
    
    else if(Command_word == 0x0202)// 处理查询CH1数据命令
    {
        float result_1=ADC_Get_CH1_Voltage()*g_ch1_ratio;  //读取滑动变阻器的电压值
        // printf("reslt_0=%0.2f\r\n",result_0);
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0202;
        response_value.Message_length=0x04;
        response_value.Protocol_version=0x02;
        Float_To_Bytes_BigEndian(result_1, response_value.Content);
    }
    else if(Command_word == 0x0221)  //查询特定通道数据（此处为外部 ADC 的 PT100）
    {

    }
    else if(Command_word == 0x0241)  //设置 CH0 变比
    {
        // 从接收帧中获取浮点数（4字节，大端序IEEE754）
        uint8_t float_bytes[4];
        float_bytes[0] = receive_value.Content[0];
        float_bytes[1] = receive_value.Content[1];
        float_bytes[2] = receive_value.Content[2];
        float_bytes[3] = receive_value.Content[3];
        
        // 将大端序字节数组转换为浮点数
        float ratio = Bytes_To_Float_BigEndian(float_bytes);
        
        // printf("Set CH0 Ratio: %.2f\r\n", ratio);
        
        // 调用变比设置函数
        ADC_Set_CH0_Ratio(ratio);
        
        // 构建应答帧（OK）
        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0241;   // 设置CH0变比
        response_value.Message_length = 0x01;   // 1字节数据
        response_value.Protocol_version = 0x02;
        response_value.Content[0] = 0xFF;       // OK
        
        // 保存到Flash
        parameter.ch0_rate=g_ch0_ratio;
        Save_Parameter();
    }

    else if(Command_word == 0x0242)  //设置 CH1 变比
    {
        // 从接收帧中获取浮点数（4字节，大端序IEEE754）
        uint8_t float_bytes[4];
        float_bytes[0] = receive_value.Content[0];
        float_bytes[1] = receive_value.Content[1];
        float_bytes[2] = receive_value.Content[2];
        float_bytes[3] = receive_value.Content[3];
        
        // 将大端序字节数组转换为浮点数
        float ratio = Bytes_To_Float_BigEndian(float_bytes);
        
        printf("Set CH1 Ratio: %.2f\r\n", ratio);
        
        // 调用变比设置函数
        ADC_Set_CH1_Ratio(ratio);
        
        // 构建应答帧（OK）
        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0242;   // 设置CH1变比
        response_value.Message_length = 0x01;   // 1字节数据
        response_value.Protocol_version = 0x02;
        response_value.Content[0] = 0xFF;       // OK
        
        // 保存到Flash
        parameter.ch1_rate=g_ch1_ratio;
        Save_Parameter();
    }
    else if(Command_word == 0x0261)//设置数据上报时间间隔
    {
        if(receive_value.Content[0]==0x01)
        {
            time_interval=1000;
        }
        else if (receive_value.Content[0]==0x02)
        {
            time_interval=3000;
        }
        else if (receive_value.Content[0]==0x03)
        {
            time_interval=5000;
        }
        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0261;   // 命令字
        response_value.Message_length = 0x01;   // 1字节数据
        response_value.Protocol_version = 0x02;
        response_value.Content[0] = 0xFF;       // OK
    }
    else if(Command_word == 0x0301)//设置 DAC 输出电压
   {
       int value=receive_value.Content[0]<<8|receive_value.Content[1];
    //    printf("value=%d\r\n",value);
       DAC_Set_Output((int)value);
    //    printf("result=%0.2f\r\n",ADC_Get_CH1_Voltage());
       response_value.Start_marker=0xA5B6;
       response_value.Device_ID=parameter.DeviceID;
       response_value.Frame_type=0x02;
       response_value.Command_word=0x0301;
       response_value.Message_length=0x01;
       response_value.Protocol_version=0x02;
       response_value.Content[0]=0xff;
   }
   else if (Command_word == 0x0302)//定时自动上报数据开始（批量上报仅 CH0、CH1）
   {
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
        appState=APP_STATE_AUTO_SAMPLING;
   }
   else if (Command_word == 0x0303)//定时自动上报数据停止
   {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x02;
        response_value.Command_word=0x0303;
        response_value.Message_length=0x01;
        response_value.Protocol_version=0x02;
        response_value.Content[0]=0xff;
        appState=APP_STATE_IDLE;
   }
   else if (Command_word == 0x0400)//读取阈值参数（批量读取仅 CH0、CH1）
   {

        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0400;
        response_value.Message_length = 0x08;   // 1字节数据
        response_value.Protocol_version = 0x02;
        Float_To_Bytes_BigEndian(ch0_threshold, response_value.Content);
        Float_To_Bytes_BigEndian(ch1_threshold, response_value.Content+4);
   }
   else if (Command_word == 0x0401)//读取 CH0 阈值参数
   {
        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0401;
        response_value.Message_length = 0x04;   // 1字节数据
        response_value.Protocol_version = 0x02;
        printf("ch0_threshold=%0.2f\r\n",ch0_threshold);
        Float_To_Bytes_BigEndian(ch0_threshold, response_value.Content);
   }
   else if (Command_word == 0x0402)//读取 CH1 阈值参数
   {
        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0402;
        response_value.Message_length = 0x04;   // 1字节数据
        response_value.Protocol_version = 0x02;
        printf("ch1_threshold=%0.2f\r\n",ch1_threshold);
        Float_To_Bytes_BigEndian(ch1_threshold, response_value.Content);
   }
   else if (Command_word == 0x0411)//写入 CH0 阈值参数
   {
        // 从接收帧中获取浮点数（4字节，大端序IEEE754）
        uint8_t float_bytes[4];
        float_bytes[0] = receive_value.Content[0];
        float_bytes[1] = receive_value.Content[1];
        float_bytes[2] = receive_value.Content[2];
        float_bytes[3] = receive_value.Content[3];
        
        // 将大端序字节数组转换为浮点数
        ch0_threshold = Bytes_To_Float_BigEndian(float_bytes);

        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0411;
        response_value.Message_length = 0x01;   // 1字节数据
        response_value.Protocol_version = 0x02;
        response_value.Content[0] = 0xff;

        // 保存到Flash
        parameter.ch0_threshold=ch0_threshold;
        Save_Parameter();
   }
   else if (Command_word == 0x0412)//写入 CH1 阈值参数
   {
        // 从接收帧中获取浮点数（4字节，大端序IEEE754）
        uint8_t float_bytes[4];
        float_bytes[0] = receive_value.Content[0];
        float_bytes[1] = receive_value.Content[1];
        float_bytes[2] = receive_value.Content[2];
        float_bytes[3] = receive_value.Content[3];
        
        // 将大端序字节数组转换为浮点数
        ch1_threshold = Bytes_To_Float_BigEndian(float_bytes);

        response_value.Start_marker = 0xA5B6;
        response_value.Device_ID = parameter.DeviceID;
        response_value.Frame_type = 0x02;       // 应答帧
        response_value.Command_word = 0x0412;
        response_value.Message_length = 0x01;   // 1字节数据
        response_value.Protocol_version = 0x02;
        response_value.Content[0] = 0xff;

        // 保存到Flash
        parameter.ch1_threshold=ch1_threshold;
        Save_Parameter();
   }
    else if(Command_word==0xFFFF)//上位机广播寻找设备
    {
        response_value.Start_marker=0xA5B6;
        response_value.Device_ID=parameter.DeviceID;
        response_value.Frame_type=0x05;
        response_value.Command_word=0x8888;
        response_value.Message_length=0x00;
        response_value.Protocol_version=0x02;
        response_value.Content[0]=0x00;
    }
   

}
