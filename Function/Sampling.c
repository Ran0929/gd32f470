#include "Sampling.h"
//采样

Sampling_channel ch0,ch1;

//初始化采样通道
void init_Sampling_channel(Sampling_channel *ch)
{
    if (ch == NULL) return;
    memset(ch, 0, sizeof(Sampling_channel));  //所有字节置0
    ch->rate = 1;
    ch->threshold =3.3;
}