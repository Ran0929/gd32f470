#include "Flash.h"
#include "Sampling.h"
#include "HeaderFiles.h"

void Save_Parameter(void)
{
    fmc_unlock(); // 解锁

    // 擦除页面每页4KB
    fmc_page_erase(PARAM_PAGE);
    // 写入ID
    fmc_halfword_program(PARAM_FLASH_ADDR,parameter.DeviceID);
    //写入固件版本
    fmc_byte_program(PARAM_FLASH_ADDR+2,parameter.Ver1);
    fmc_byte_program(PARAM_FLASH_ADDR+3,parameter.Ver2);
    fmc_byte_program(PARAM_FLASH_ADDR+4, parameter.Ver3);
    fmc_byte_program(PARAM_FLASH_ADDR+5,parameter.Ver4);
    //波特率写入
    fmc_word_program(PARAM_FLASH_ADDR+8,parameter.Baud_rate);
    //ch0-ch1变比和阈值
    fmc_word_program(PARAM_FLASH_ADDR+12, *(uint32_t*)&parameter.ch0.rate);
    fmc_word_program(PARAM_FLASH_ADDR+16, *(uint32_t*)&parameter.ch1.rate);
    fmc_word_program(PARAM_FLASH_ADDR+20, *(uint32_t*)&parameter.ch0.threshold);
    fmc_word_program(PARAM_FLASH_ADDR+24, *(uint32_t*)&parameter.ch1.threshold);
    //存储告警记录
    for(int i=0;i<10;i++)
    {
        fmc_byte_program(PARAM_FLASH_ADDR+28+16*i, *(uint8_t*)&parameter.record[i].time.year);
        fmc_byte_program(PARAM_FLASH_ADDR+29+16*i, *(uint8_t*)&parameter.record[i].time.month);
        fmc_byte_program(PARAM_FLASH_ADDR+30+16*i, *(uint8_t*)&parameter.record[i].time.date);
        fmc_byte_program(PARAM_FLASH_ADDR+31+16*i, *(uint8_t*)&parameter.record[i].time.hour);
        fmc_byte_program(PARAM_FLASH_ADDR+32+16*i, *(uint8_t*)&parameter.record[i].time.minute);
        fmc_byte_program(PARAM_FLASH_ADDR+33+16*i, *(uint8_t*)&parameter.record[i].time.second);

        fmc_byte_program(PARAM_FLASH_ADDR+34+16*i, *(uint8_t*)&parameter.record[i].channel_id);
        fmc_word_program(PARAM_FLASH_ADDR+36+16*i, *(uint32_t*)&parameter.record[i].threshold.threshold);
        fmc_word_program(PARAM_FLASH_ADDR+40+16*i, *(uint32_t*)&parameter.record[i].sampled_value);
    }
    
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
    parameter.ch0.rate = *(volatile float*)(addr+12);
    parameter.ch1.rate = *(volatile float*)(addr+16);
    parameter.ch0.threshold = *(volatile float*)(addr+20);
    parameter.ch1.threshold = *(volatile float*)(addr+24);
    //读告警记录
    for(int i=0;i<10;i++)
    {
        parameter.record[i].time.year = *(uint8_t*)(PARAM_FLASH_ADDR+28+16*i);
        parameter.record[i].time.month = *(uint8_t*)(PARAM_FLASH_ADDR+29+16*i);
        parameter.record[i].time.date = *(uint8_t*)(PARAM_FLASH_ADDR+30+16*i);
        parameter.record[i].time.hour = *(uint8_t*)(PARAM_FLASH_ADDR+31+16*i);
        parameter.record[i].time.minute = *(uint8_t*)(PARAM_FLASH_ADDR+32+16*i);
        parameter.record[i].time.second = *(uint8_t*)(PARAM_FLASH_ADDR+33+16*i);

        parameter.record[i].channel_id = *(volatile uint8_t*)(PARAM_FLASH_ADDR+34+16*i);
        parameter.record[i].threshold.threshold = *(volatile float*)(PARAM_FLASH_ADDR+36+16*i);
        parameter.record[i].sampled_value = *(volatile float*)(PARAM_FLASH_ADDR+40+16*i);
    }
}
