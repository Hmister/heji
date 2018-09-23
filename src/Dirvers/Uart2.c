/**====================================================================*
*						STC15ϵ�е�Ƭ������2����
*----------------------------------------------------------------------*
*�� �� ��:    Uart2.c
*�ļ�����:    ����2�շ��Լ��˿ڵ�ӳ��
*����汾:    V1.0
*Ӳ������:    STC15W4K32S4
*�� �� ��:    ����  526293582@qq.com(����) 13477037750(�ֻ�)
*����޸�:    2017/06/23 11:55
*����˵��:    �жϵķ�ʽʵ�ַ��ͺͽ���,ʵ�ֶ˿ڿ�ӳ��
-----------------------------------------------------------------------*
*�ⲿ��Դ:	 
1.	1ms��ʱ������,���ڽ����ǳ�ʱ�ж�
2.	Uart2_PORT������,�������ƴ���0ʹ���ĸ��˿�
*�޸�˵��:
V1.0 ---------> V1.1
1.	P3.7 P1.7��Ϊ����0���ͽź����Ҫ���ó�ǿ�������,P1.6��Ϊ��������ű���Ҫ
	���ÿ�©,��������ʹ��
-----------------------------------------------------------------------*
*Ӳ������:
			WZ01_SN_A_V1.0			USBתTTLģ��			PC��������
				GND		-------------	GND			
				P3.0	-------------	TX	
				P3.1	-------------	RX		-------		USB		(Uart2_PORT=1)
				P1.6	-------------	TX	
				P1.7	-------------	RX		-------		USB		(Uart2_PORT=2)
				P3.6	-------------	TX	
				P3.7	-------------	RX		-------		USB		(Uart2_PORT=����)
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"
#include "Uart2.h"


#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3
#define S2_S0 0x01              //P_SW2.0


static bit busy=0;

uchar gUart2_RecvFlag = 0;
uchar gUart2_RecvBuf[40] = 0;

void Send2Data(BYTE dat);
void Send2String(char *s);


void Uart2Init()
{
#if (Uart2_PORT==1)
    P_SW2 &= ~S2_S0;            //S2_S0=0 (P0.0/RxD3, P0.1/TxD3)
#elif (Uart2_PORT==2)	
	P_SW2 |= S2_S0;             //S2_S0=1 (P5.0/RxD3_2, P5.1/TxD3_2)
#endif
//ʹ�ö�ʱ��2��Ϊ�����ʷ�����		
    S2CON = 0x50;               //����ʹ��,B5����Ϊ1
	
    T2L = (65536 - (FOSC/4/BAUD));   	//���ò�������װֵ
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR |= 0x10;                		//ʹ�ܶ�ʱ��2
	AUXR |= 0x04;						//T2Ϊ1Tģʽ

	
    IE2 |= 0x01;                 		//ʹ�ܴ���2�ж�
    EA = 1;
	
	Send2String("Uart2 Init!\r\n");
}

/*----------------------------
UART �жϷ������
-----------------------------*/
void Uart2() interrupt 8 using 1
{
	static uchar i = 1;
	uchar temp = 0;
//	Send1Data('5');
	if(S2CON & S2RI)
	{
		S2CON &= ~S2RI;
		if(S2BUF == '\n')			//��������"\r\n"ʱ,ȥ��"\n",���ִ�����������Ὣ�����滻��"\r\n"
		{
			temp = S2BUF;
			return;
		}
		gUart2_RecvBuf[i] = S2BUF;
		if(gUart2_RecvBuf[i] == '\r')
		{
			gUart2_RecvBuf[0] = i;	//��0���ֽڱ����ִ��ĳ���
			gUart2_RecvFlag = 1;
			i = 1;
		}
		else
		{
			i++;
		}		
	}
    if(S2CON & S2TI)
    {
		S2CON &= ~S2TI;
		busy = 0;
    }
}

/*----------------------------
���ʹ�������
----------------------------*/
void Send2Data(BYTE dat)
{
//	Send1Data('3');
	while(busy);
	busy = 1;
	S2BUF = dat;         
//	Send1Data('4');
}

void Send2Datas(uchar *s,uint len)
{
	
	BUZZER = !BUZZER;
	if(len==0){
		Send2String(s);
		return;
	}
	while(len){
		Send2Data(*s++);
		if(*s == '\r' || *s == '\0')
			break;
		len--;
	}
}

/*----------------------------
�����ַ���
----------------------------*/
void Send2String(char *s)
{
    while (*s)                  //����ַ���������־
    {
        Send2Data(*s++);         //���͵�ǰ�ַ�
    }
}

//*-------------------------------------------------------------
//*����˵��:	���ڽ��պ���,ʹ��T3��ʱ�ж�
//*��ڲ���:	*Data--���ջ���,Ҫ�㹻��
//*				len--��Ҫ���յĳ���,0��ʾ������󳤶�
//*				time--��ʱ����,��λms
//*�� �� ֵ:	�յ����ֽ���
//*ע    ��:ʹ�ô˺���ʱ,����Ҫ���жϹ��ܹر�,�����־Ϊ���ж��������
//*-------------------------------------------------------------
extern unsigned long System1MsCnt;
int Recv2Bytes(unsigned char *Data,int len,unsigned int time)
{
	unsigned int i=0;
	unsigned long curtime = System1MsCnt;
	while(1){
		if(S2CON & S2RI){			
			Data[i++] = S2BUF;
			S2CON &= ~S2RI;
		}
		if(len!=0 && i==len+1){			//���յ���ָ���ĳ���
			return len;
		}
		if(System1MsCnt-curtime>time){
			break;						//��ʱ,���ն����ֽھͷ��ض����ֽ�
		}
	}
	return i;
}