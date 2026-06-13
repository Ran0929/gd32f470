#ifndef RESPONSE_H
#define RESPONSE_H
//应答组帧

#include <stdint.h>


//通用帧格式结构体
typedef struct General_frame_format {
    uint16_t Start_marker;              //起始标志
    uint16_t Device_ID;                 //设备ID
    uint16_t Frame_type;                //帧类型
    uint16_t Command_word;              //命令字
    uint16_t Message_length;            //消息长度
    uint16_t Protocol_version;          //协议版本
    uint8_t Content[32];                //内容
    uint16_t CRC16;                     //CRC16
    uint16_t Closing_symbol;            //结束标志
}GFF;

typedef enum
{
    Command,//命令下发帧
    Response,//应答帧
    Heartbeat,//心跳帧
    Exception,//异常报警、错误应答帧
}Frame_type;

//帧答复状态枚举
typedef enum
{
    T_NONE,
    WAIT,
    OK,

}Response_status;

extern GFF response_value;
extern Response_status response_status;
extern Frame_type frame_type;

void Frame_assembly(void);
void init_response_value(Frame_type frame_type);

#endif
