#include "Flash.h"

#include "HeaderFiles.h"

SystemParams_t g_sys_params;

void Flash_read(void)
{
    // 读出Flash参数
    SystemParams_t *p = (SystemParams_t *)PARAM_FLASH_ADDR;
    
    if (p->Flash_save_win == Flash_Flag_CODE) {
        // Flash有效，读取所有字段
        g_sys_params.Flash_save_win = p->Flash_save_win;
        g_sys_params.ch0_ratio = p->ch0_ratio;
        g_sys_params.ch1_ratio = p->ch1_ratio;
        g_sys_params.ch0_threshold = p->ch0_threshold;
        g_sys_params.ch1_threshold = p->ch1_threshold;
        g_sys_params.device_id = p->device_id;
        g_sys_params.baudrate_code = p->baudrate_code;
    } else {
        // Flash无效，使用默认值
        g_sys_params.Flash_save_win = Flash_Flag_CODE;
        g_sys_params.ch0_ratio = 1.0f;
        g_sys_params.ch1_ratio = 1.0f;
        g_sys_params.ch0_threshold = 3.0f;
        g_sys_params.ch1_threshold = 3.0f;
        g_sys_params.device_id = 0x0001;
        g_sys_params.baudrate_code = 13;  // 19200
    }
}

void Flash_save(void)
{
    // 更新Flash保存的数
    g_sys_params.Flash_save_win = Flash_Flag_CODE;
    
    // 写入Flash参数
    fmc_unlock();
    fmc_page_erase(PARAM_FLASH_ADDR);
    fmc_ready_wait(FMC_TIMEOUT_COUNT);

    uint32_t *buf = (uint32_t *)&g_sys_params;
    for (int i = 0; i < sizeof(SystemParams_t) / 4; i++) {
        fmc_word_program(PARAM_FLASH_ADDR + i * 4, buf[i]);
        fmc_ready_wait(FMC_TIMEOUT_COUNT);
    }

    fmc_lock();
}

