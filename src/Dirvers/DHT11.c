/**====================================================================*
*							DHT11��������
*----------------------------------------------------------------------*
*�� �� ��:    DHT11.c
*�ļ�����:    ��ȡDHT11��ʪ�ȴ�����������
*����汾:    V1.1
*Ӳ������:    
*�� �� ��:    ����  526293582@qq.com(����) 13477037750(�ֻ�)
*����޸�:    2017/06/09 16:33
*����˵��:    
-----------------------------------------------------------------------*
*DHT11ʹ�÷���:
1.	��ʼ�ź�:��Ƭ������������19ms,Ȼ������������,����"����DHT11����Ҫ������"
2.	Ӧ���ź�:DHT11�յ���ʼ�źź�,����������80us,Ȼ��������80us,����"��׼����
	����������",���������������ź���
3.	�����ź�:ÿһbit���ݶ���50us�͵�ƽʱ϶��ʼ,�ߵ�ƽ�ĳ��̶�������λ��0(26us��28us)
	����1(70us),�����һbit���ݴ�����Ϻ�DHT11��������50us,���������������������
	�������״̬
=======================================================================*/

#include "DHT11.h"
#include "Uart0.h"

//
//��ʪ�ȶ���
uchar ucharFLAG=0,uchartemp=0;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;


//��ʪ�ȴ���
void COM(void)    // ��ʪд��
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);	//50us�͵�ƽ,��ʾ�źŵĿ�ʼ,��������ź�0(����28us)����ʱ,Ȼ���������ߵĸߵ�
		DelayUs(30);						//***�߼������ǲ���,��Լ��ʱ53us,�˺�����ʱ����ȷ,��������Ҫ�ù��߲���!!
        uchartemp=0;
        if(DATA_PIN){
			uchartemp=1;	
		}else{
		}
        
		ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);		//���,��������߳����ߵ�ƽ�ܳ�ʱ���,��������ȻΪ��,��ô˵�����ݴ��������(�������������ߵ�)

		if(ucharFLAG==1){
			break;    
		}
        
		ucharcomdata<<=1;
        ucharcomdata|=uchartemp; 
    }    
}

unsigned int DHT11(void)   	//��ʪ��������
{
	SetPinMode(P1M0,P1M1,POMOD,1);			//����P1.1Ϊ���
	
    DATA_PIN=0;
    DelayMs(19);  			//>18MS
    DATA_PIN=1; 

	SetPinMode(P1M0,P1M1,INMOD,1);			//����P1.1Ϊ����
	
    DelayUs(10);
    DelayUs(10);                        
    DelayUs(10);
    DelayUs(10);  
	
    if(!DATA_PIN) 
    {		
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);		//DHT11����80us��Ӧ�����ź�
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++); 		//DHT11����80us��Ӧ�����ź�,����ľ��������ź���
        COM();
        ucharRH_data_H_temp=ucharcomdata;		//ʪ�ȸ߰�λ
        COM();
        ucharRH_data_L_temp=ucharcomdata;		//ʪ�ȵͰ�λ
        COM();
        ucharT_data_H_temp=ucharcomdata;		//�¶ȸ߰�λ
        COM();
        ucharT_data_L_temp=ucharcomdata;		//�¶ȵͰ�λ
        COM();
        ucharcheckdata_temp=ucharcomdata;		//У��λ
        DATA_PIN=1; 
		
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
		
        if(uchartemp==ucharcheckdata_temp)
        {
            ucharRH_data_H=ucharRH_data_H_temp;
            ucharRH_data_L=ucharRH_data_L_temp;
            ucharT_data_H=ucharT_data_H_temp;
            ucharT_data_L=ucharT_data_L_temp;
            ucharcheckdata=ucharcheckdata_temp;
        }    
    }	
//	Send0Data(ucharT_data_H<<8);Send0Data(ucharRH_data_H<<0);
	return (ucharT_data_H<<8)|(ucharRH_data_H<<0);
}
