#include "Flash.h"

#include "HeaderFiles.h"

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
    uint32_t addr = PARAM_FLASH_ADDR;
    //设备ID
    uint16_t dev_id = *(volatile uint16_t*)addr;
    parameter.DeviceID = (dev_id != 0xFFFF) ? dev_id : 0x0001;
    //固件版本
    uint8_t v1 = *(volatile uint8_t*)(addr+2);
    parameter.Ver1 = (v1 != 0xFF) ? v1 : 0x02;
    uint8_t v2 = *(volatile uint8_t*)(addr+3);
    parameter.Ver2 = (v2 != 0xFF) ? v2 : 0x00;
    uint8_t v3 = *(volatile uint8_t*)(addr+4);
    parameter.Ver3 = (v3 != 0xFF) ? v3 : 0x01;
    uint8_t v4 = *(volatile uint8_t*)(addr+5);
    parameter.Ver4 = (v4 != 0xFF) ? v4 : 0x00;
    //波特率
    uint32_t baud = *(volatile uint32_t*)(addr+8);
    parameter.Baud_rate = (baud != 0xFFFFFFFF) ? baud : 19200;
    
    //ch0~ch1变比和阈值
    parameter.ch0_rate = *(volatile float*)(addr+12);
    parameter.ch1_rate = *(volatile float*)(addr+16);
    parameter.ch0_threshold = *(volatile float*)(addr+20);
    parameter.ch1_threshold = *(volatile float*)(addr+24);
}
