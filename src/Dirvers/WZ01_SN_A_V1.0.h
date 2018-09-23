#include "STC15.h"
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>



//--------------------------------��������-----------------------------------
#define u8			unsigned char
#define uchar 		unsigned char 
#define uint 		unsigned int
#define ulong 		unsigned long
#define uint16 		unsigned int
#define uint8 		unsigned char
#define BYTE		unsigned char
#define WORD 		unsigned int


//--------------------------------�����ļ�-----------------------------------
#include "Delay.h"
#include "OLED.h"
#include "Uart1.h"										//���Դ���,����
#include "../SHARE/FG_Tool.h"


//--------------------------------ϵͳ����-----------------------------------
#define FOSC 11059200L          						//ϵͳƵ��
#define BAUD 115200             						//���ڲ�����

#define T1MS (65536-FOSC/1000)      					//1Tģʽ
//#define T1MS (65536-FOSC/12/1000) 					//12Tģʽ

#define UART1_PORT					1					//<1=>P3.0/P3.1  <2=>P3.6/P3.7 <else=>P1.6/P1.7
#define UART2_PORT					1					//<1=>P1.0/P1.1  <2=>P4.6/P4.7
#define UART3_PORT					1					//<1=>P0.0/P0.1  <2=>P5.0/P5.1
#define UART4_PORT					1					//<1=>P0.2/P0.3  <2=>P5.2/P5.3

//--------------------------------���Ŷ���-----------------------------------
#define BUZZER   	 				P45                 //����������
#define DATA_IO0					P10 
#define DATA_IO1					P11 
#define DATA_IO2					P02
#define DATA_IO3					P03

#define IOMOD						0					//׼˫���,������
#define POMOD						1					//�������,Push Out
#define INMOD						2					//��������
#define ODMOD						3					//��©,Open Drain ˫���,��Ҫ����������

#define KEY1						P46
#define LED2						P22
#define LED3						P23


//--------------------------------���ܺ���-----------------------------------
#define SIZEOF(arr)					(sizeof(arr)/sizeof(arr[0]))

#define BUZZER_INIT()				P4M1 &= ~(1<<5);P4M0 |= (1<<5);BUZZER = 0;
#define BUZZER_ON()					BUZZER = 1;
#define BUZZER_OFF()				BUZZER = 0;

#define LED_INIT()					P2M1 &= ~(3<<2);P2M0 |= (3<<2);LED2 = 0;LED3 = 0;
#define LED2_ON()					LED2 = 0;
#define LED2_OFF()					LED2 = 1;
#define LED3_ON()					LED3 = 0;
#define LED3_OFF()					LED3 = 1;

#define KEY_INIT()					P4M0 &= ~(1<<6);P4M1 |= (1<<6);	//����P46Ϊ���뷽ʽ



#define Bit(bit)					(1<<(bit))
#define SetBit(reg,bit) 			(reg|=(1<<(bit)))
#define ClrBit(reg,bit) 			(reg&=~(1<<(bit)))
#define RevBit(reg,bit) 			(reg^=(1<<(bit)))
#define ReadBit(reg,bit)			((reg>>(bit))&1)
#define HBit8(v) 					(((v)>>8)&0xff)
#define LBit8(v)					((v)&0xff)
#define HBit16(v) 					(((v)>>16)&0xffff)
#define LBit16(v) 					((v)&0xffff) 
//��������ģʽ,Mode[0..3]���α�ʾ׼˫��,����,����,��©,Bits��ʾҪ���õĽ�,����֧�ֶ�������Ľ�ͬʱ���ó�ͬ���Ĺ���
#define SetPinMode(M1,M0,Mode,Bits)	{Mode&Bit(1)?SetBit(M1,Bits):ClrBit(M1,Bits);Mode&Bit(0)?SetBit(M0,Bits):ClrBit(M0,Bits);}


//--------------------------------�ⲿ����-----------------------------------
extern unsigned char code ascii_table_8x16[95][16];
extern unsigned char code Chinese_text_16x16[];
extern unsigned char code Chinese_code_16x16[];

extern unsigned long System1MsCnt;						//ϵͳ1ms������

//--------------------------------�ⲿ����-----------------------------------


	