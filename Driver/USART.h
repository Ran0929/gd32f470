#ifndef USART_H
#define USART_H

#include "HeaderFiles.h"

extern uint32_t SET_Baud_rate;             //波特率
#define RX_BUF_SIZE  256		// 接收缓冲区大小

extern char rx_buf[RX_BUF_SIZE];	// 接收缓冲区
extern volatile uint16_t rx_head;	// 接收缓冲区头指针
extern volatile uint16_t rx_tail;	// 接收缓冲区尾指针
extern uint32_t SET_Baud_rate;

void USART0_Config(void);
void USART0_SendData(uint16_t *buf, uint16_t len);
int usart0_getchar(void);
void collect_line(void);
void process_line(char *line);

#endif
