#include "Response.h"
#include "parsing.h"
#include "CRC.h"
//应答组帧

Response_status response_status=T_NONE;

GFF response_value;
Frame_type frame_type=Response;
uint8_t tx_cmd[128];
char cmd_ascii[128];

void init_response_value(Frame_type frame_type)
{
    response_value.Start_marker=0xA5B6;
    response_value.Device_ID=parameter.DeviceID;
    //应答帧
    if(frame_type==Response)
    {
        response_value.Frame_type=0x02;
    }
    //心跳帧
    else if(frame_type==Heartbeat)
    {
        response_value.Frame_type=0x05;
    }
    //异常报警、错误应答帧
    else if(frame_type==Exception)
    {
        response_value.Frame_type=0xFF;
    }
    response_value.Command_word=0x0111;
    //执行函数需要修改消息长度
    response_value.Message_length=0x02;
    response_value.Protocol_version=0x02;
    //内容给执行函数填execute_Command_word();
    for(int i=0;i<16;i++)
    response_value.Content[i]=0x00;
}

//组帧
void Frame_assembly()
{
    int count=0;
    tx_cmd[count++]=response_value.Start_marker>>8;
    tx_cmd[count++]=response_value.Start_marker;
    tx_cmd[count++]=response_value.Device_ID>>8;
    tx_cmd[count++]=response_value.Device_ID;
    tx_cmd[count++]=response_value.Frame_type;
    tx_cmd[count++]=response_value.Command_word>>8;
    tx_cmd[count++]=response_value.Command_word;
    tx_cmd[count++]=response_value.Message_length;
    tx_cmd[count++]=response_value.Protocol_version;
    // 动态处理n字节数据
    for(int i=0;i<response_value.Message_length;i++)
    {
        tx_cmd[count++] = response_value.Content[i];
    }

    //CRC校验
    response_value.CRC16=CRC16_transform(tx_cmd,count);
    // printf("crc:%04X\r\n",response_value.CRC16);
    tx_cmd[count++]=response_value.CRC16>>8;
    tx_cmd[count++]=response_value.CRC16;

    response_value.Closing_symbol=0xB6A5;
    tx_cmd[count++]=response_value.Closing_symbol>>8;
    tx_cmd[count++]=response_value.Closing_symbol;

//    printf("start....\r\n");

    // HextoASCII(cmd_hex,frame_tail,cmd_ascii);
    for(int i=0;i<count;i++)
    printf("%02X",tx_cmd[i]);
    printf("\r\n");

//    printf("end...\r\n");
}
