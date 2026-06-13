#include "USART.h"

//波特率设置
uint32_t SET_Baud_rate=19200;

char rx_buf[RX_BUF_SIZE];	// 接收缓冲区
volatile uint16_t rx_head = 0;	// 接收缓冲区头指针
volatile uint16_t rx_tail = 0;	// 接收缓冲区尾指针

// 串口0配置
void USART0_Config()
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART0);

	gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_9|GPIO_PIN_10);
	
	//PA9 == TxD
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_9);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);

	//PA10 == RxD
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_10);
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	
	usart_deinit(USART0);  //复位串口
	usart_baudrate_set(USART0, SET_Baud_rate); //定义波特率
	// usart_parity_config(USART0,USART_PM_NONE); //定义校验方式
	// usart_word_length_set(USART0,USART_WL_8BIT); //定义字长
	// usart_stop_bit_set(USART0,USART_STB_1BIT); //定义停止位
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
	
	// usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
	// usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
	
	nvic_irq_enable(USART0_IRQn,0,0);					// 中断使能
	usart_interrupt_enable(USART0,USART_INT_RBNE);		// 串口缓存中断使能
	usart_enable(USART0);
}

void USART0_SendData(uint16_t *buf, uint16_t len)
{
	uint16_t t;
	
	for(t=0;t<len;t++)
	{
		while(RESET == usart_flag_get(USART0,USART_FLAG_TC));
		usart_data_transmit(USART0,buf[t]);
	
	}
	while(RESET == usart_flag_get(USART0,USART_FLAG_TC));

}

int fputc(int ch, FILE *f)
{
	usart_data_transmit(USART0,(uint8_t)ch);
	while(RESET == usart_flag_get(USART0,USART_FLAG_TBE));
	return ch;

}

// 非阻塞获取一个字符
int usart0_getchar(void)
{
    if (rx_head == rx_tail) return -1;
    char ch = rx_buf[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
    return (int)ch;
}

// 从环形缓冲区不断取字符，拼成一行，遇到换行符则调用 process_line
void collect_line(void)
{
    static char line_buf[128];
    static uint16_t idx = 0;
    int ch;

    while ((ch = usart0_getchar()) != -1) {
        if (ch == '\r' || ch == '\n') {
            if (idx > 0) {
                line_buf[idx] = '\0';
                process_line(line_buf);
                idx = 0;
            }
        } else {
            if (idx < sizeof(line_buf) - 1) {
                line_buf[idx++] = (char)ch;
            } else {
                // 行太长，丢弃并重置
                idx = 0;
            }
        }
    }
}
// 处理一行完整的字符串（命令或交互输入）
void process_line(char *line)
{
    //回显
    for(int k=0;k<RX_BUF_SIZE; k++)
    {
        if(rx_buf[k] != '\0')
        {
            while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
            usart_data_transmit(USART0,rx_buf[k]);
        }
    }
    // 这一步至关重要，它告诉串口助手“这行字发完了，请显示”
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    usart_data_transmit(USART0, '\r'); // 回车
    
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    usart_data_transmit(USART0, '\n'); // 换行
}
