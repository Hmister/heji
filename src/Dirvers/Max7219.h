#ifndef __HAL_MAX7219_H_
#define __HAL_MAX7219_H_

#include "WZ01_SN_A_V1.0.h"
#include <string.h>
#include "Delay.h"
#include "OLED.h"

#define Max7219_pinDIN	DATA_IO0
#define Max7219_pinCLK	DATA_IO1
#define Max7219_pinCS	DATA_IO2

extern uchar LedDisp[];

void LHNX2HLSX(uint8* imLH,uint8* exHL);
void NX2SX(uint8* imNX,uint8* exSX);

void Write_Max7219_byte(uchar DATA);
void Write_Max7219(uchar address1,uchar dat1,uchar address2,uchar dat2);
void Init_MAX7219_Port(void);
void Init_MAX7219(void);

void Display_Max7219(uint n);
void Display_Max7219_Move(uint n,uint flag);
void Display_Dianzhen(uint num,uint time);
void ChineseUpdate(uchar* text);

void fg_LedDisplay_4TS(uchar* hz);	//显示特殊字符,手动添加的字模
void fg_LedDisplay(uchar* hz);		//**显示字符串hz的内容,支持汉字/字符/数字,使用oled字库	
void fg_LedDisplay_4HZ(uchar* hz);	//一次最多显示4个汉字	
void fg_LedDisplay_2ZF(uchar* zf);	//显示汉字和字符,但一次只能显示1个汉字或者2个字符
void fg_LedDisplay_HZ(uchar* hz);	//显示任意长度的汉字,长度不限
#endif 
