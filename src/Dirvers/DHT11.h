#ifndef __DHT11_H__
#define __DHT11_H__

#include "WZ01_BR13_A_V1.0.h"
#include "Delay.h"


#define DATA_PIN 		DATA_IO1

void COM(void);
unsigned int DHT11(void);
#endif