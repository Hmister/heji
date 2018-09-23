/**====================================================================*
*						16*16������������ʾ����
*----------------------------------------------------------------------*
*�� �� ��:    hal_max7219.c
*�ļ�����:    ʵ����MAX7219оƬ�������Լ�16*16������ʾ��Ӧ�ú���,�û�ֱ��
*			  ���ü��ɼ򵥵�ʵ�ֵ���ĺ���/�ַ�/���ֵȵ���ʾ
*����汾:    V1.1
*Ӳ������:    WZ-SN-V1.1  WZ-S02-V1.1
*�� �� ��:    ���� 526293582@qq.com 13477037750
*����޸�:    2017/06/07 10:19
*����˵��:    ʵ��16*16������ַ�����ʾ����
-----------------------------------------------------------------------*
*
=======================================================================*/
#include "Max7219.h"

uchar LedDisp[32*4]=
{
	0xF8,0xF8,0x0C,0xC4, 0x0C,0xF8,0xF0,0x00, 0x03,0x07,0x0C,0x08, 0x0C,0x07,0x03,0x00,		//-0-		  	ASCII�룺0X30
	0x00,0x10,0x18,0xFC, 0xFC,0x00,0x00,0x00, 0x00,0x08,0x08,0x0F, 0x0F,0x08,0x08,0x00,		//-1-
};
//��byte�ĵ�posλȡ����
uint8 gbit(uint8 byte,uint8 pos)
{
	return (1<<pos)&byte?1:0;
}
//��im[index]��im[index+7]�����ֽڵĵ�posλȡ����,���һ���ֽڷ���
uint8 gbyte(int index,uint8 pos,uchar* im)
{
	uint8 res=0;
	int i=0;
	for(i=0;i<8;i++){
		res |= gbit(im[index+i],pos)<<7-i;
	}
	return res;
}
//*==============================================|=====================*
//*����ԭ��:	
//*��������:	������ʽ����ת������ʽ˳��
//*��    ��:    �Ѳ���,����
//*ת �� ǰ:
//0x20,0x20,0x24,0x24,0x24,0xA4,0x24,0x24,
//0x20,0xFF,0x20,0x22,0x2C,0x20,0x20,0x00,
//0x40,0xC0,0x7E,0x40,0x40,0x3F,0x22,0x22,
//0x20,0x03,0x0C,0x10,0x20,0x40,0xF8,0x00,/*���� ���� "��",0*/
//*ת �� ��:
//0x00,0x00,0x3F,0x00,0x00,0xFF,0x00,0x04,
//0x04,0x27,0x24,0x24,0x24,0x27,0xF8,0x40,
//0x40,0x50,0x48,0x48,0x40,0xFE,0x40,0x40,
//0x40,0x40,0x20,0x22,0x12,0x8A,0x06,0x02,/*���� ˳�� "��",0*/
//*==============================================|=====================*
void LHNX2HLSX(uint8* imLH,uint8* exHL)
{
	uint8 i=0;
	for(i=0;i<8;i++){
		exHL[7-i] = gbyte(0,7-i,imLH);
	}
	for(i=0;i<8;i++){
		exHL[15-i] = gbyte(16,7-i,imLH);
	}
	for(i=0;i<8;i++){
		exHL[23-i] = gbyte(8,7-i,imLH);
	}
	for(i=0;i<8;i++){
		exHL[31-i] = gbyte(24,7-i,imLH);
	}
}

//*==============================================|=====================*
//*����ԭ��:	
//*��������:	������ʽ����ת������ʽ˳��
//*��    ��:    �Ѳ���,����
//*˵    ��:	00100000b -> 00000100b
//*==============================================|=====================*
unsigned char Reverse(unsigned char uch) 
{ 
	unsigned char retValue = 0;    
	int i;  
	retValue |= (uch & 1);   
	for (i = 1; i < 8; i++){ 
		retValue <<= 1; 
		retValue |= (((1 << i) & uch) >> i);   
	}  
	return retValue; 
} 
//*==============================================|=====================*
//*����ԭ��:	
//*��������:	������ʽ����ת������ʽ˳��
//*��    ��:    �Ѳ���,����
//*ת �� ǰ:
//0x00,0x00,0xFC,0x00,0x00,0xFF,0x00,0x20,
//0x20,0xE4,0x24,0x24,0x24,0xE4,0x1F,0x02,
//0x02,0x0A,0x12,0x12,0x02,0x7F,0x02,0x02,
//0x02,0x02,0x04,0x44,0x48,0x51,0x60,0x40,/*���� ���� "��",0*/
//*ת �� ��:
//0x00,0x00,0x3F,0x00,0x00,0xFF,0x00,0x04,
//0x04,0x27,0x24,0x24,0x24,0x27,0xF8,0x40,
//0x40,0x50,0x48,0x48,0x40,0xFE,0x40,0x40,
//0x40,0x40,0x20,0x22,0x12,0x8A,0x06,0x02,/*���� ˳�� "��",0*/
//*==============================================|=====================*
void NX2SX(uint8* imNX,uint8* exSX)
{
	uint8 i=0;
	for(i=0;i<32;i++){
		exSX[i] = Reverse(imNX[i]);
	}
}

//*==============================================|=====================*
//*����ԭ��:	
//*��������:	��OLED���ֿ��в��Һ���text�ľ���λ��
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
uint16 ChinesePos(uchar *text) //Chinese_code_16x16[ChinesePos("Ψ")];
{
	uint8 i = 0;
	uint16 address = 1;
	while(Chinese_text_16x16[i] > 0x7e)	  // >0x7f��˵������ASCII���ַ�
	{
		if(Chinese_text_16x16[i] == text[0])
		{
			if(Chinese_text_16x16[i + 1] == text[0 + 1])
			{
				address = i * 16;
				return address;
			}
		}
		i += 2; 
	}
	return -1;
}
//*==============================================|=====================*
//*����ԭ��:	
//*��������:	ʹ��OLED�ֿ��е���ģ��ˢ��LedDisp��ģ����
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void ChineseUpdate(uchar* text)
{    
    uint8 j = 0;
	uint16 pos=0;
    while(text[j] != '\0')
    {
		pos = ChinesePos(text+j);
        if(pos != -1){
			LHNX2HLSX((uchar*)Chinese_code_16x16+pos,LedDisp+j/2*32);
		}
		j += 2;
	}
}
//*==============================================|=====================*
//*����ԭ��:	
//*��������:	��8*16���ַ���ģ,������ʽ����ת������ʽ˳��
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void CharModChange(uchar* imChar,uchar* exChar)
{
	uint8 i=0;
	for(i=0;i<8;i++){
		exChar[7-i] = gbyte(0,7-i,imChar);
	}
	for(i=0;i<8;i++){
		exChar[15-i] = gbyte(8,7-i,imChar);
	}
}
//*==============================================|=====================*
//*����ԭ��:	
//*��������:	��ɶ�16*16������ģ��8*16�ַ���ģ��ת��(ʹ��oled�ֿ�)
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void CharUpdata(uchar* text)
{
	uint8 j=0;
	uint16 pos=0;	
	uchar* ptrAsc = (void *)ascii_table_8x16[0];
	if(text[0] > 0x7e){			//����	
		pos = ChinesePos(text);
		if(pos != -1){
			LHNX2HLSX((uchar*)Chinese_code_16x16+pos,LedDisp);
		}
	}else{						//ASCII�ַ�	
		j=text[0]-0x20;			//�õ��ַ����ֿ��еľ���λ��
		memset(LedDisp,0,sizeof(LedDisp));
		CharModChange(ptrAsc+j*16,LedDisp);
		
		if(text[1] != '\0'){	//2���ַ�
			j=text[1]-0x20;
			CharModChange(ptrAsc+j*16,LedDisp+16);
		}
	}
}

//*==============================================|=====================*
//*����ԭ��:	
//*��������:	��ʼ��IO��,��������MAX7219
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void Init_MAX7219_Port(void)
{
//    P1SEL &= 0x70;            //��P00Ϊ��ͨIO��
//    P1DIR |= 0x07;            //����Ϊ���ģʽ  
	P1M1 &= ~(3<<2);
	P1M0 |= (3<<2);
	P0M1 &= ~(1<<2);
	P0M0 |= (1<<2);
    
    Max7219_pinCS=1;
    Max7219_pinCLK=1;
    Max7219_pinDIN=1;
}
//*==============================================|=====================*
//*����ԭ��:	
//*��������:	��ʼ��MAX7219
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void Init_MAX7219(void)
{
    uchar  i;
	Init_MAX7219_Port();
    Max7219_pinCS=0;
    for(i=0;i<4;i++)
    {
      Write_Max7219_byte(0x09);  
      Write_Max7219_byte(0x00);    
    }
    Max7219_pinCS=1;
    DelayUs(10);
    Max7219_pinCS=0;
    for(i=0;i<4;i++)
    {
      Write_Max7219_byte(0x0a); 
      Write_Max7219_byte(0x03);    
    }
    Max7219_pinCS=1;
    DelayUs(10);
     Max7219_pinCS=0;
    for(i=0;i<4;i++)
    {
      Write_Max7219_byte(0x0b); 
      Write_Max7219_byte(0x07);    
    }
    Max7219_pinCS=1;
    DelayUs(10);
    Max7219_pinCS=0;
    for(i=0;i<4;i++)
    {
      Write_Max7219_byte(0x0c); 
      Write_Max7219_byte(0x01);    
    }
    Max7219_pinCS=1;
    DelayUs(10);
    Max7219_pinCS=0;
    for(i=0;i<4;i++)
    {
      Write_Max7219_byte(0x0f); 
      Write_Max7219_byte(0x00);    
    }
    Max7219_pinCS=1;
    DelayUs(10);
}

void Write_Max7219_byte(uchar DATA)         
{
	uchar i;  
	Max7219_pinCS=0;
	for(i=8;i>=1;i--)
	{		  
		Max7219_pinCLK=0;  
		if(DATA&0x80) Max7219_pinDIN=1;   
		else Max7219_pinDIN=0;
		DATA=DATA<<1;
		Max7219_pinCLK=1;
	}                                 
}

void Display_Max7219(uint n)
{
  uint i,j;
  for(i=1;i<9;i++)
  {   
     Max7219_pinCS=0;
     for(j=0;j<4;j++)
     {
         Write_Max7219_byte(i);
         Write_Max7219_byte(LedDisp[(n-1-j)*8+(i-1)]);
         DelayUs(10);          
     }
     Max7219_pinCS=1; 
  }
}

void Display_Max7219_Move(uint n,uint flag)
{
  uint i,j,m;
  if(flag==1)
  {
    for(m=8;m>0;m--)
    {
        for(i=1;i<9;i++)
        {   
            Max7219_pinCS=0;
            for(j=0;j<4;j++)
            {
                  Write_Max7219_byte(i);
                  Write_Max7219_byte((LedDisp[(n-1-j)*8+(i-1)])>>m);
                  DelayUs(10);        
            }
            Max7219_pinCS=1; 
        }
        DelayMs(30);
     }
  }
  else
  {
    for(m=0;m<9;m++)
    {
        for(i=1;i<9;i++)
        {   
            Max7219_pinCS=0;
            for(j=0;j<4;j++)
            {
                  Write_Max7219_byte(i);
                  Write_Max7219_byte((LedDisp[(n-1-j)*8+(i-1)])<<m);
                  DelayUs(10);         
            }
            Max7219_pinCS=1; 
        }
        DelayMs(30);
     }
  }
}

void Display_Dianzhen(uint num,uint time)
{
  Display_Max7219_Move(num,1); 
  Display_Max7219(num);
  DelayMs(time);					//��ʾ��ͣ��ʱ��
  Display_Max7219_Move(num,0);
}

//*==============================================|=====================*
//*����ԭ��:	
//*��������:	ʹ�õ�����ʾ�����ַ���,���һ����ʾ4�������ַ�,���ڲ���,ֱ������ģ
//*				�ִ����ֶ������ģ
//*˵    ��:	��Ҫ��LedDisp�ĳ�[32*4]
//*��    ��:    δ����
//*==============================================|=====================*
void fg_LedDisplay_4TS(uchar* hz)
{ 
	uint8 i=0;
	uint8 hzLen=strlen((char*)hz);
	if(hzLen>8) return;	
	for(i=0;i<hzLen/2;i++){
		Display_Dianzhen(4+i*4,300);//��ʾ��ͣʱ��,Ĭ��300ms		
	}
}


//*==============================================|=====================*
//*����ԭ��:	
//*��������:	ʹ�õ�����ʾ�����ַ���,���һ����ʾ4������
//*˵    ��:	��Ҫ��LedDisp�ĳ�[32*4]
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void fg_LedDisplay_4HZ(uchar* hz)
{ 
	uint8 i=0;
	uint8 hzLen=strlen((char*)hz);
	if(hzLen>8) return;	
	ChineseUpdate(hz);				//������4������
	for(i=0;i<hzLen/2;i++){
		Display_Dianzhen(4+i*4,300);//��ʾ��ͣʱ��,Ĭ��300ms		
	}
}



//*==============================================|=====================*
//*����ԭ��:	
//*��������:	ʹ�õ�����ʾ�����ַ������Ȳ���,ֻ��������ʾ�ֿ��д��ڵĺ���
//*				���ʺ���ʾASCII�ַ�
//*˵    ��:	��Ҫ��LedDisp�ĳ�[32*4]
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void fg_LedDisplay_HZ(uchar* hz)
{ 
	uint8 len=0,j=0,i=0;
	uint8 temp[8];
	int hzLen=strlen((char*)hz);
	while(hzLen>0){
		len = strlen((char*)hz+j*8);	//8��ʾ4�����ֵĳ���
		len = len>8?8:len%8;
		strncpy(temp,(char*)hz+j*8,len);		ChineseUpdate(temp);	
		
		for(i=0;i<len/2;i++){
			Display_Dianzhen(4+i*4,300);//��ʾ��ͣʱ��,Ĭ��300ms		
		}		
		
		j++;
		hzLen -= 8;
	}
}

//*==============================================|=====================*
//*����ԭ��:	
//*��������:	��ʾ�ַ�����,�����ʾ2���ַ�(1������),�����˳��Ƚ�������ʾ
//*				�ʺ���ʾһ��
//*˵    ��:	��Ҫ��LedDisp�ĳ�[32*1]
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void fg_LedDisplay_2ZF(uchar* zf)
{ 
	if(strlen(zf)>2) return;
	
	CharUpdata(zf);
	Display_Dianzhen(4,300);
}

//*==============================================|=====================*
//*����ԭ��:	
//*��������:	��ʾ�ַ�����,����ʾ���⺺���ַ���ɵ��ַ���
//*				�ʺ���ʾ����
//*˵    ��:	��Ҫ��LedDisp�ĳ�[32*4]
//*��    ��:    �Ѳ���,����
//*==============================================|=====================*
void fg_LedDisplay(uchar* hz)
{ 
	uint8 len=0,j=0,i=0;
	uint8 temp[2];
	int hzLen=strlen((char*)hz);
	while(hzLen>0){
		len = strlen((char*)hz+j*2);	
		len = len>=2?2:len%2;
		memset(temp,0,2);
		strncpy(temp,(char*)hz+j*2,len);
		CharUpdata(temp);	
		
		Display_Dianzhen(4,300);//��ʾ��ͣʱ��,Ĭ��300ms						
		
		j++;
		hzLen -= 2;
	}
}