#ifndef __FLASH_H
#define __FLASH_H

#include "HeaderFiles.h"
#include "gd32f4xx.h"

#define PARAM_FLASH_ADDR    0x08010000
#define Flash_Flag_CODE     0x12345678

void Save_Parameter(void);
void Read_Parameter(void);

#endif

