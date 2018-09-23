/**====================================================================*
*						STC15ϵ�е�Ƭ������0����
*----------------------------------------------------------------------*
*�� �� ��:    Uart.c
*�ļ�����:    ����0�շ��Լ��˿ڵ�ӳ��
*�����汾:    V1.1
*Ӳ������:    STC15W4K32S4
*�� �� ��:    ����  526293582@qq.com(����) 13477037750(�ֻ�)
*����޸�:    2017/06/13 11:08
*����˵��:    ��ѯ�ķ�ʽʵ�ַ��ͺͽ���,ʵ�ֶ˿ڿ�ӳ��
-----------------------------------------------------------------------*
*�ⲿ��Դ:	 
1.	1ms��ʱ������,���ڽ����ǳ�ʱ�ж�
2.	UART0_PORT������,�������ƴ���0ʹ���ĸ��˿�
*�޸�˵��:
V1.0 ---------> V1.1
1.	P3.7 P1.7��Ϊ����0���ͽź����Ҫ���ó�ǿ�������,P1.6��Ϊ��������ű���Ҫ
	���ÿ�©,��������ʹ��
-----------------------------------------------------------------------*
*Ӳ������:
			WZ01_ZL_A_V1.0			USBתTTLģ��			PC��������
				GND		-------------	GND			
				P3.0	-------------	TX	
				P3.1	-------------	RX		-------		USB		(UART0_PORT=1)
				P1.6	-------------	TX	
				P1.7	-------------	RX		-------		USB		(UART0_PORT=2)
				P3.6	-------------	TX	
				P3.7	-------------	RX		-------		USB		(UART0_PORT=����)
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"


#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

static bit busy;

uchar gUart1_RecvFlag = 0;			
uchar gUart1_RecvBuf[40] = 0;

void Send1Data(BYTE dat);
void Send1String(char *s);


void Uart1Init()
{
#if (UART0_PORT==1)
    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
#elif (UART0_PORT==2)
	ACC = P_SW1;
	ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
	ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
	P_SW1 = ACC;  

//����P3.7Ϊǿ����ģʽ,������Ϊ���ڷ��ͽ�,������������
	P3M1 &= ~(1<<7);
	P3M0 |= (1<<7); 

#elif (UART0_PORT==3)
	ACC = P_SW1;
	ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
	ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
	P_SW1 = ACC;  
	
//����P16Ϊ׼˫���,P17Ϊ�������,��Ϊ����0ʹ��ʱ,һ��Ҫ����
	P1M1 &= ~(1<<6);
	P1M0 &= ~(1<<6);
	
	P1M1 &= ~(1<<7);
	P1M0 |= (1<<7);
#endif
	
    SCON |= 0x50;                //8λ�ɱ䲨����
    AUXR |= 0x40;                //��ʱ��1Ϊ1Tģʽ, ʹ��|=���´��ڲ���ʹ��
	AUXR &= 0xFE;				//�����ʷ�����ѡ��: bit0<1=>��ʱ��2 <0=>��ʱ��1
	
    TMOD = 0x00;                //��ʱ��1Ϊģʽ0(16λ�Զ�����)
    TL1 = (65536 - (FOSC/4/BAUD));   //���ò�������װֵ
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //��ʱ��1��ʼ����
    ES = 1;                   //ʹ�ܴ����ж�  �ر��ж�,����Recv0Bytes()�����쳣
    EA = 1;

    Send1String("STC15F2K60S2\r\nUart0 Init!\r\n");
}



/*----------------------------
UART �жϷ������
-----------------------------*/
void Uart1() interrupt 4 using 1
{
	static uchar i = 1;
	uchar temp = 0;
	if(RI)
	{
		RI = 0;
		if(SBUF == '\n')			//��������"\r\n"ʱ,ȥ��"\n",���ִ������������Ὣ�����滻��"\r\n"
		{
			temp = SBUF;
			return;
		}
		gUart1_RecvBuf[i] = SBUF;
		if(gUart1_RecvBuf[i] == '\r')
		{
			gUart1_RecvBuf[0] = i;	//��0���ֽڱ����ִ��ĳ���
			gUart1_RecvFlag = 1;
			i = 1;
		}
		else
		{
			i++;
		}		
	}
    if (TI)
    {

    }
}

/*----------------------------
���ʹ�������
----------------------------*/
void Send1Data(BYTE dat)
{
    SBUF = dat;                 //д���ݵ�UART���ݼĴ���
	while(TI==0);				//�ȴ��������,Ȼ���ٽ������жϱ�־����
	TI = 0;	
}

void Send1Datas(uchar *s,uint len)
{
	uint i=len;
//	BUZZER = !BUZZER;
	if(len==0){
		Send1String(s);
		return;
	}
	while(i){
		Send1Data(*s++);
//		if(*s == '\r' || *s == '\0')
//			break;
		i--;
	}
}

/*----------------------------
�����ַ���
----------------------------*/
void Send1String(char *s)
{
    while (*s)                  //����ַ���������־
    {
        Send1Data(*s++);         //���͵�ǰ�ַ�
    }
}

//*-------------------------------------------------------------
//*����˵��:	���ڽ��պ���,ʹ��T3��ʱ�ж�
//*��ڲ���:	*Data--���ջ���,Ҫ�㹻��
//*				len--��Ҫ���յĳ���,0��ʾ������󳤶�
//*				time--��ʱ����,��λms
//*�� �� ֵ:	�յ����ֽ���
//*-------------------------------------------------------------
extern unsigned long System1MsCnt;
int Recv1Bytes(unsigned char *Data,int len,unsigned int time)
{
	unsigned int i=0;
	unsigned long curtime = System1MsCnt;
	while(1){
		if(RI == 1){			
			Data[i++] = SBUF;
			RI = 0;
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