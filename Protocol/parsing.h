#ifndef PARSING_H
#define PARSING_H
//帧解析
#include "HeaderFiles.h"
#include "Response.h"


//帧接收状态枚举
typedef enum {
    R_NONE=0,
    start,
    end,
    finish,
}parsing_Status;


extern char rx_cmd[128];
extern uint8_t cmd_hex[128];
extern uint16_t frame_tail;
extern parsing_Status parsingStatus;
extern GFF receive_value;


int Frame_analysis(void);
uint8_t ASCIItoHex(char high, char low);
char HextoASCII(uint8_t *hex_buf,uint16_t hex_len,char *ascii_buf);

void deinit_GFF(void);

#endif
