/**====================================================================*
*						STC15ϵ�е�Ƭ������3����
*----------------------------------------------------------------------*
*�� �� ��:    Uart.c
*�ļ�����:    ����3�շ��Լ��˿ڵ�ӳ��
*����汾:    V1.0
*Ӳ������:    STC15W4K32S4
*�� �� ��:    ����  526293582@qq.com(����) 13477037750(�ֻ�)
*����޸�:    2017/06/23 11:55
*����˵��:    �жϵķ�ʽʵ�ַ��ͺͽ���,ʵ�ֶ˿ڿ�ӳ��
-----------------------------------------------------------------------*
*�ⲿ��Դ:	 
1.	1ms��ʱ������,���ڽ����ǳ�ʱ�ж�
2.	UART3_PORT������,�������ƴ���0ʹ���ĸ��˿�
*�޸�˵��:
V1.0 ---------> V1.1
1.	P3.7 P1.7��Ϊ����0���ͽź����Ҫ���ó�ǿ�������,P1.6��Ϊ��������ű���Ҫ
	���ÿ�©,��������ʹ��
-----------------------------------------------------------------------*
*Ӳ������:
			WZ01_SN_A_V1.0			USBתTTLģ��			PC��������
				GND		-------------	GND			
				P3.0	-------------	TX	
				P3.1	-------------	RX		-------		USB		(UART3_PORT=1)
				P1.6	-------------	TX	
				P1.7	-------------	RX		-------		USB		(UART3_PORT=2)
				P3.6	-------------	TX	
				P3.7	-------------	RX		-------		USB		(UART3_PORT=����)
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"
#include "Uart1.h"

#define S3_S0 0x02              //P_SW2.1
#define S3RI  0x01              //S3CON.0
#define S3TI  0x02              //S3CON.1
#define S3RB8 0x04              //S3CON.2
#define S3TB8 0x08              //S3CON.3


static bit busy=0;

uchar gUart3_RecvFlag = 0;
uchar gUart3_RecvBuf[40] = 0;

void Send3Data(BYTE dat);
void Send3String(char *s);


void Uart3Init()
{
#if (UART3_PORT==1)
    P_SW2 &= ~S3_S0;            //S3_S0=0 (P0.0/RxD3, P0.1/TxD3)
#elif (UART3_PORT==2)	
	P_SW2 |= S3_S0;             //S3_S0=1 (P5.0/RxD3_2, P5.1/TxD3_2)
#endif
//ʹ�ö�ʱ��2��Ϊ�����ʷ�����		
//    S3CON = 0x10;               //����ʹ��,B5����Ϊ1
//	
//    T2L = (65536 - (FOSC/4/BAUD));   	//���ò�������װֵ
//    T2H = (65536 - (FOSC/4/BAUD))>>8;
//    AUXR |= 0x10;                		//ʹ�ܶ�ʱ��2
//	AUXR |= 0x04;						//T2Ϊ1Tģʽ
	
//ʹ�ö�ʱ��3��Ϊ�����ʷ�����
	S3CON = 0x50;               //8λ�ɱ䲨����,ѡ��ʱ��3,�������
	
    T3L = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    T3H = (65536 - (FOSC/4/BAUD))>>8;
    T4T3M |= 0x02;              //��ʱ��3Ϊ1Tģʽ
    T4T3M |= 0x08;              //��ʱ��3��ʼ��ʱ
	
    IE2 |= 0x08;                 		//ʹ�ܴ���3�ж�
    EA = 1;
	
	Send1String("Uart3 Init!\r\n");
}

/*----------------------------
UART �жϷ������
-----------------------------*/
void Uart3() interrupt 17 using 1
{
	static uchar i = 1;
	uchar temp = 0;
//	Send1Data('5');
	if(S3CON & S3RI)
	{
		S3CON &= ~S3RI;
		if(S3BUF == '\n')			//��������"\r\n"ʱ,ȥ��"\n",���ִ�����������Ὣ�����滻��"\r\n"
		{
			temp = S3BUF;
			return;
		}
		gUart3_RecvBuf[i] = S3BUF;
		if(gUart3_RecvBuf[i] == '\r')
		{
			gUart3_RecvBuf[0] = i;	//��0���ֽڱ����ִ��ĳ���
			gUart3_RecvFlag = 1;
			i = 1;
		}
		else
		{
			i++;
		}		
	}
    if(S3CON & S3TI)
    {
		S3CON &= ~S3TI;
		busy = 0;
    }
}

/*----------------------------
���ʹ�������
----------------------------*/
void Send3Data(BYTE dat)
{
//	Send1Data('3');
	while(busy);
	busy = 1;
	S3BUF = dat;         
//	Send1Data('4');
}

void Send3Datas(uchar *s,uint len)
{
	
	BUZZER = !BUZZER;
	if(len==0){
		Send3String(s);
		return;
	}
	while(len){
		Send3Data(*s++);
		if(*s == '\r' || *s == '\0')
			break;
		len--;
	}
}

/*----------------------------
�����ַ���
----------------------------*/
void Send3String(char *s)
{
    while (*s)                  //����ַ���������־
    {
        Send3Data(*s++);         //���͵�ǰ�ַ�
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
int Recv3Bytes(unsigned char *Data,int len,unsigned int time)
{
	unsigned int i=0;
	unsigned long curtime = System1MsCnt;
	while(1){
		if(S3CON & S3RI){			
			Data[i++] = S3BUF;
			S3CON &= ~S3RI;
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