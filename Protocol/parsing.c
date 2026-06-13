#include "parsing.h"
#include "main.h"
#include "RTC.h"
//帧解析



//帧命令
char rx_cmd[128];
//帧命令转16进制
uint8_t cmd_hex[128];

//接收数据帧结构体
GFF receive_value;

//帧头帧尾
uint16_t frame_head=0;
uint16_t frame_tail=0;

//帧当前解析状态
parsing_Status parsingStatus=R_NONE;



//将收到的数据进行帧解析
int Frame_analysis(void)
{
    
    //帧解析
    if(parsingStatus==R_NONE)
    {
        for(int i=0;i<RX_BUF_SIZE;i++)
        {
            //查找帧头
            if((rx_buf[i]=='A')&&(rx_buf[i+1]=='5')&&(rx_buf[i+2]=='B')&&(rx_buf[i+3]=='6'))
            {
                frame_head=i;
                // printf("i_start=%d\r\n",i);
                parsingStatus=start;
                break;
            }
        }
    }
    else if(parsingStatus==start)
    {
        for(int i=frame_head;i<RX_BUF_SIZE;i++)
        {
            //查找帧尾
            if((rx_buf[i]=='B')&&(rx_buf[i+1]=='6')&&(rx_buf[i+2]=='A')&&(rx_buf[i+3]=='5'))
            {
                
                frame_tail=i;
                // printf("i_end=%d\r\n",i);
                parsingStatus=end;
                break;
            }
        }
    }
    else if(parsingStatus==end)//完成帧头帧尾解析，开始存储命令
    {
        int j=0;
        for(int i=frame_head;i<frame_tail+4;i++)
        {
            rx_cmd[j++]=rx_buf[i];
        }
        rx_cmd[j]='\0';
        // printf("cmd：%s\r\n",rx_cmd);

        //ascii码转16进制
        int hex_len = j / 2; // HEX长度
        for(int k=0;k<hex_len;k++)
        {
            cmd_hex[k] = ASCIItoHex(rx_cmd[2*k], rx_cmd[2*k+1]);
            frame_tail=k;
        }
        
         //写入结构体
        receive_value.Start_marker=(cmd_hex[0]<<8)|cmd_hex[1];
        receive_value.Device_ID=(cmd_hex[2]<<8)|cmd_hex[3];
        receive_value.Frame_type=cmd_hex[4];
        receive_value.Command_word=(cmd_hex[5]<<8)|cmd_hex[6];
        receive_value.Message_length=cmd_hex[7];
        receive_value.Protocol_version=cmd_hex[8];
        // 动态处理n字节数据
        for(int i = 0; i < receive_value.Message_length; i++)
        {
            receive_value.Content[i] = cmd_hex[9+i];
        }
        receive_value.CRC16=(cmd_hex[frame_tail-3]<<8)|cmd_hex[frame_tail-2];
        receive_value.Closing_symbol=(cmd_hex[frame_tail-1]<<8)|cmd_hex[frame_tail];

        // printf("Start_marker=%04X\r\n",receive_value.Start_marker);
        // printf("Device_ID=%04X\r\n",receive_value.Device_ID);
        // printf("Frame_type=%02X\r\n",receive_value.Frame_type);
        // printf("Command_word=%04X\r\n",receive_value.Command_word);
        // printf("Message_length=%02X\r\n",receive_value.Message_length);
        // printf("Protocol_version=%02X\r\n",receive_value.Protocol_version);
        // printf("Content:");
        // for(int i = 0; i < receive_value.Message_length; i++)
        // {
        //     printf("%02X ", receive_value.Content[i]);
        // }
        // printf("\r\n");
        // printf("CRC16=%04X\r\n",receive_value.CRC16);
        // printf("Closing_symbol=%04X\r\n",receive_value.Closing_symbol);

        // printf("cmd_hex: ");
        // for(int k = 0; k < 40; k++)
        // {
        //     printf("%02X ", cmd_hex[k]);
        // }
        // printf("\r\n");
        
        
        //完成帧解析状态
        // 解析完成，清空缓冲区
        memset(rx_buf,0,RX_BUF_SIZE);
        rx_head = 0;
        frame_head = 0;
        frame_tail = 0;

        parsingStatus=finish;

    }
    return parsingStatus;
}

//ASCII码转16进制
uint8_t ASCIItoHex(char high, char low)
{
    uint8_t val = 0;
    if(high >= '0' && high <= '9') val = (high - '0') << 4;
    else if(high >= 'A' && high <= 'F') val = (high - 'A' + 10) << 4;
    else if(high >= 'a' && high <= 'f') val = (high - 'a' + 10) << 4;

    if(low >= '0' && low <= '9') val |= (low - '0');
    else if(low >= 'A' && low <= 'F') val |= (low - 'A' + 10);
    else if(low >= 'a' && low <= 'f') val |= (low - 'a' + 10);

    return val;
}

//16进制转ASCII码
char HextoASCII(uint8_t *hex_buf,uint16_t hex_len,char *ascii_buf)
{
    uint16_t i;

    for(i = 0; i < hex_len; i++)
    {
        uint8_t high = (hex_buf[i] >> 4) & 0x0F;
        uint8_t low  = hex_buf[i] & 0x0F;

        ascii_buf[2*i] =
            (high < 10) ? ('0'+high) : ('A'+high-10);

        ascii_buf[2*i+1] =
            (low < 10) ? ('0'+low) : ('A'+low-10);
    }

    ascii_buf[2*hex_len] = '\0';
    return 0;
}



//初始化帧结构体
void deinit_GFF(void)
{
    //初始化接收帧结构体
    receive_value.Start_marker=0;
    receive_value.Device_ID=0;
    receive_value.Frame_type=0;
    receive_value.Command_word=0;
    receive_value.Message_length=0;
    receive_value.Protocol_version=0;
    for(int i = 0; i < receive_value.Message_length; i++)
    {
        receive_value.Content[i] = 0;
    }
    receive_value.CRC16=0;
    receive_value.Closing_symbol=0;
}
















