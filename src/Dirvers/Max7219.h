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

void fg_LedDisplay_4TS(uchar* hz);	//��ʾ�����ַ�,�ֶ���ӵ���ģ
void fg_LedDisplay(uchar* hz);		//**��ʾ�ַ���hz������,֧�ֺ���/�ַ�/����,ʹ��oled�ֿ�	
void fg_LedDisplay_4HZ(uchar* hz);	//һ�������ʾ4������	
void fg_LedDisplay_2ZF(uchar* zf);	//��ʾ���ֺ��ַ�,��һ��ֻ����ʾ1�����ֻ���2���ַ�
void fg_LedDisplay_HZ(uchar* hz);	//��ʾ���ⳤ�ȵĺ���,���Ȳ���
#endif 
