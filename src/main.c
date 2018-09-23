/**====================================================================*
*					16*16 ���󴮿ڸ���ʵ��
*----------------------------------------------------------------------*
*�� �� ��:    main.c
*�ļ�����:    
*����汾:    V1.0
*Ӳ������:    WZ01_WN_A_V1.0
*�� �� ��:    ����  526293582@qq.com(����) 13477037750(�ֻ�)
*����޸�:    2017/06/11 15:29
*����˵��:    ����ѭ����ʾ����0���յ�������(���ֻ��ַ�)
1.	�����޸ĵ�����������ʱ,��Ҫ�ȴ�����ʾ����(���´���ʾ֮��Ŀ�϶ʱ��),����
	���պ͵�������ʾ���������ķ�ʽ,��������ʾ��ʱ���ܽ���������
2.	���Ե������ڽ��ճ�ʱ(��ڲ���3),�����Ӵ�������ʱ��
3.	���ڳ�ʼ���󽫴�ӡ��ǰ֧�ֵĺ����ֿ�(ASC�ַ�,֧������)
4.  ��Ӽ�ֵ���Ƶ�����ʾ��ͬ������
*�� �� ��:
1.	�����޸�����ʱ,����������ַ�������ź���,����ʾ�쳣,��"Ψabc��",��������
	���ֱ�����ַ�������ӿո���뼴��������ʾ
-----------------------------------------------------------------------*
*ע������:
1.	STC15W4K32S4ϵ�е�оƬ,�ϵ��������PWM��ص�IO�ھ�Ϊ����̬,�轫��Щ������
	Ϊ׼˫��ڻ�ǿ����ģʽ��������ʹ��
	���IO: P0.6/P0.7/P1.6/P1.7/P2.1/P2.2 P2.3/P2.7/P3.7/P4.2/P4.4/P4.5
2.	STC15��Ƭ��ʹ��PxM0��PxM1�����Ĵ�������IO��ģʽ(0:������ 1:ǿ���� 2:���� 3:��©)
3.	����0����ʹ��P3.0/P3.1 P3.6/P3.7 P1.6/P1.7����ʹ��UART0_PORT����ѡ��
4.  ʹ�ô���APP��ֵ1���ֵ2���Ƶ�����ʾ������ʱ,�����ڵ�����ʾ��Ϻ���ܰ���һ����ֵ
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"	
#include "Uart1.h"
#include "Uart3.h"
#include "Max7219.h"

ulong System1MsCnt = 0;				//ϵͳ1ms������
ulong SystemSecond = 0;

void Timer0Init()
{
    AUXR |= 0x80;                   //��ʱ��0Ϊ1Tģʽ
//  AUXR &= 0x7f;                   //��ʱ��0Ϊ12Tģʽ

    TMOD = 0x00;                    //���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
    TL0 = T1MS;                     //��ʼ����ʱֵ
    TH0 = T1MS >> 8;
    TR0 = 1;                        //��ʱ��0��ʼ��ʱ
    ET0 = 1;                        //ʹ�ܶ�ʱ��0�ж�
    EA = 1;
}
//*==============================================|=====================*
//*����ԭ��:    
//*��������:    ��ʱ��0�жϷ�����(������ÿ1MS����һ��)
//*==============================================|=====================*
void Timer0_ISR() interrupt 1 using 1
{
    static uint counter0=0;
	counter0++;
	System1MsCnt++;
	
	if(System1MsCnt%1000==0){			
//		BUZZER = !BUZZER;
	}
}


void main(void)
{


	int len=0;
	int temp=0;
	//��ʼ��������
	BUZZER_INIT();
	//ʱ���ʼ��
	Timer0Init();
	//��ʱ��1�Ĳ�����
	Uart1Init();
	//��ʱ��3�Ĳ�����
	Uart3Init();

	Send1String("֧�ֺ��ֿ�:\r\n");
	Send1String(Chinese_text_16x16);
	
	Init_MAX7219();
	
	OledInit();	
	OledClear();
	OledDispString(1,1,"  ������ʾʵ��  ");
	fg_LedDisplay("����ְҵ����ѧԺ    ");
	BUZZER_ON();
	DelayMs(2000);
	BUZZER_OFF();
	
      while(1){
			
		//1.����������ʾ����	
		if(KEY1==0)
		{
			DelayMs(10);
			if(KEY1==0)
			{
				Send1String("KEY ");
				temp ++;			
				OledClearHalf(2);
				//����ֵΪż��ʱ������2����0��
				if(temp%2==0)
				{								 
			     	OledSprintf(3,1,"���ܴ���");
		        	fg_LedDisplay("���ǰ���2  ");
				    LED2_OFF();
					BUZZER_ON();
					DelayMs(100);
					BUZZER_OFF();
				
				}
				else
				{
				    fg_LedDisplay();
								 
					OledSprintf(3,1,"���ܴ���");
					fg_LedDisplay("���ǰ���1  ");
					LED2_ON();
					BUZZER_ON();
					DelayMs(300);
					BUZZER_OFF();
				}			
			}
		}



		//2.���ر���������ʾ����
		if(gUart3_RecvFlag==1)	//�жϴ����Ƿ��յ������ַ���
		{
		    Send3String(gUart3_RecvBuf+1);
		
			OledClearHalf(2);
			if(strncmp(gUart3_RecvBuf+1,"#key 1",6)==0)//��ֵ��ʽ������ʾ����
			{							
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();	   
				OledSprintf(3,1,"���");
				fg_LedDisplay("��ƽ�ֹͨ��");
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();
			
			}
			else if(strncmp(gUart3_RecvBuf+1,"#key 2",6)==0)
			{
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();
				OledSprintf(3,1,"�̵�");
				fg_LedDisplay("�̵�����ͨ��");
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();
		
			}
		memset(gUart3_RecvBuf,0,SIZEOF(gUart3_RecvBuf));
		gUart3_RecvFlag=0;

	}
	  


	
		

		
		
		
		 

		//3.���Դ����޸���ʾ����,��������ʽ,ÿ����ѭ����ռ��CPUʱ��,�ᵼ�������������Ʋ���
		if(gUart1_RecvFlag==1)																	  
		{
			fg_LedDisplay(gUart1_RecvBuf+1);
			memset(gUart1_RecvBuf,0,SIZEOF(gUart1_RecvBuf));
			gUart1_RecvFlag = 0;		
		}
	}
}
