#ifndef __ADC_H_
#define __ADC_H_

#include "STC15.h"


#define ADC_POWER   0x80            //ADC电源控制位
#define ADC_FLAG    0x10            //ADC完成标志
#define ADC_START   0x08            //ADC起始控制位
#define ADC_SPEEDLL 0x00            //540个时钟
#define ADC_SPEEDL  0x20            //360个时钟
#define ADC_SPEEDH  0x40            //180个时钟
#define ADC_SPEEDHH 0x60            //90个时钟

#define REF_VOLTAGE	3.3				//ADC参考电压 根据实际的来设置

void InitADC();
unsigned char GetADCResult(unsigned char ch);

#endif
