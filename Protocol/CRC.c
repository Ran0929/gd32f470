#include "CRC.h"
//CRC校验

uint16_t CRC16_transform(uint8_t *data, uint16_t len)  //用数据获得CRC的值
{
    uint16_t result = 0xFFFF ;  
    
    for (uint16_t i = 0; i < len; i++) {
        result = result ^ data[i];      // 刚开始先对数据进行异或       
        for (uint16_t j = 0; j < 8; j++) {
            if (result & 0x0001) {              // 检查最低位是否为0，
                result = (result >> 1) ^ 0xA001;   // 最低位不为0，右移一位，再异或多项式
            } else {
                result = result >> 1;              // 最低位为0，只右移
            }
        }
    }   
    return result;
}



