#ifndef __FLASH_H
#define __FLASH_H

#include "HeaderFiles.h"
#include "gd32f4xx.h"

#define PARAM_FLASH_ADDR    0x08010000
#define Flash_Flag_CODE     0x12345678


typedef struct {
    uint32_t Flash_save_win;
    float ch0_ratio;
    float ch1_ratio;
    float ch0_threshold;
    float ch1_threshold;
    uint16_t device_id;
    uint8_t baudrate_code;
} SystemParams_t;

extern SystemParams_t g_sys_params;

void Flash_read(void);
void Flash_save(void);

#endif

