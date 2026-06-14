#ifndef SAMPLING_H
#define SAMPLING_H
//采样
#include "string.h"



//采样通道结构体
typedef struct 
{
    float rate;
    float threshold;
}Sampling_channel;

extern Sampling_channel ch0,ch1;

void init_Sampling_channel(Sampling_channel *ch);

#endif
