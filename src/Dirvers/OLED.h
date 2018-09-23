#ifndef __OLED_H_
#define __OLED_H_

#include "WZ01_SN_A_V1.0.h"

#define BV(x) 	(1<<x)



#define OLED_SCL  P52  			//接口定义:lcd_sclk就是LCD的SCL   
#define OLED_SDA  P53   		//接口定义:OLED_SDA就是LCD的SDA	 

void OledStart(); 
void OledStop(); 
void OledTransfer(unsigned char data1);
void OledTransferCmd(unsigned char com);
void OledTransferData(unsigned char dat);
void OledInit();
void OledAddress(unsigned char page,unsigned char column);
void OledClear();
void OledClearHalf(unsigned char part);
void OledClearPart(unsigned char lr);
void oled_full_disp(unsigned char data1,unsigned char data2);

void Display_128x32(unsigned char page,unsigned char column,unsigned char *dp);
void DisplayGraphic_32x32(unsigned char page,unsigned char column,unsigned char *dp);
void DisplayGraphic_16x16(unsigned char page,unsigned char column,unsigned char *dp);
void DisplayGraphic_8x16(unsigned char page,unsigned char column,unsigned char *dp);
void DisplayString_8x16(unsigned int page,unsigned int column,unsigned char text[]);
void display_string_5x8(unsigned int page,unsigned int column,unsigned char reverse,unsigned char *text);
void DisplayString_16x16(unsigned char page,unsigned char column,unsigned char *text);
void DisplayString_8x16_16x16(unsigned char page,unsigned char column,unsigned char *text);

void OledDispString(uchar page,uchar column,uchar *text);
void OledPortInit( void );
void OledSprintf(uchar page,uchar column,const char *format, ...);
#endif