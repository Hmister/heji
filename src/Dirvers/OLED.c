/**====================================================================*
*							OLED(IIC 0.91��)Һ������
*----------------------------------------------------------------------*
*�� �� ��:    OLED.c
*�ļ�����:    0.91��IIC�ӿ�OLED��ʾ����������
*����汾:    V1.1(�淶�˺��������淶)
*Ӳ������:    WZ01_WN_A_V1.0
*�� �� ��:    ����  526293582@qq.com(����) 13477037750(�ֻ�)
*����޸�:    2017/06/08 16:51
*����˵��:    ʵ����ʾ�ַ��ͺ���
-----------------------------------------------------------------------*
*
=======================================================================*/

#include "OLED.h"

uchar LineBuf[18];

void OledStart() 
{ 
	OLED_SCL = 1; 
	OLED_SDA = 1; 
	OLED_SDA=0; 
	OLED_SCL=0; 
} 

void OledStop() 
{ 
	OLED_SCL=0; 
	OLED_SDA=0; 
	OLED_SDA=1; 
	OLED_SCL=1; 
} 

//��8λָ������ݵ�OLED��ʾģ��
void OledTransfer(uchar data1) 
{
	unsigned char j; 
	for(j=0;j<8;j++) 
	{ 
		OLED_SCL=0;      
		if(data1&0x80)	OLED_SDA=1;  
		else         	OLED_SDA=0;  
		data1<<=1;
		OLED_SCL=1; 
    } 
    OLED_SCL=0; 
    DelayUs(3);  //   ACKʱ��
    OLED_SCL=1; 
}

//дָ�OLED��ʾģ��
void OledTransferCmd(uchar com)   
{
	OledStart(); 
	OledTransfer(0x78); 
	OledTransfer(0x00); 
	OledTransfer(com); 
	OledStop(); 
}

//д���ݵ�OLED��ʾģ��
void OledTransferData(uchar dat)
{
	OledStart(); 
	OledTransfer(0x78); 
	OledTransfer(0x40); 
	OledTransfer(dat); 
	OledStop(); 
} 


//OLED��ʾģ���ʼ��
void OledInit()
{	
	OledPortInit();
	DelayMs(200); 
	
	OledTransferCmd(0xae);	//����ʾ
	
	OledTransferCmd(0xd5);	//����Ƶ��
	OledTransferCmd(0x80);
	
	OledTransferCmd(0xa8);	//duty����
	OledTransferCmd(0x1f);	//duty=1/32
	
	OledTransferCmd(0xd3);	//��ʾƫ��
	OledTransferCmd(0x00);	
	
	OledTransferCmd(0xa1);	//���ض�������
	OledTransferCmd(0xa6);
	OledTransferCmd(0x40);	//��ʼ��
	OledTransferCmd(0xb0);
	OledTransferCmd(0xd5);	
	OledTransferCmd(0xf0);
	
	
	OledTransferCmd(0x8d);	//��ѹ����
	OledTransferCmd(0x14);	
	
	OledTransferCmd(0x20);	//page address mode
	OledTransferCmd(0x02);
	
	OledTransferCmd(0xc8); //��ɨ��˳�򣺴��ϵ���
	OledTransferCmd(0xa1); //��ɨ��˳�򣺴�����
	
	OledTransferCmd(0xda); //sequential configuration
	OledTransferCmd(0x02);	//****************
	
	OledTransferCmd(0x81); //΢���Աȶ�,��ָ���0x81��Ҫ�Ķ����������ֵ
	OledTransferCmd(0xcf); //΢���Աȶȵ�ֵ�������÷�Χ0x00��0xff
	
	
	OledTransferCmd(0xd9); //Set Pre-Charge Period
	OledTransferCmd(0xf1);
	
	OledTransferCmd(0xdb); //Set VCOMH Deselect Level
	OledTransferCmd(0x49);
	
	OledTransferCmd(0xaf); //����ʾ
}

void OledAddress(uchar page,uchar column)
{
	column=column-1;  							//����ƽ����˵�ĵ�1�У���LCD����IC���ǵ�0�С������������ȥ1.
	page=page-1;
	OledTransferCmd(0xb0+page);   			    //����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ������ƽ����˵�ĵ�1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1
	OledTransferCmd(((column>>4)&0x0f)+0x10);	//�����е�ַ�ĸ�4λ
	OledTransferCmd(column&0x0f);				//�����е�ַ�ĵ�4λ
}

//ȫ������
void OledClear()
{
	unsigned char i,j;
 	for(j=0;j<8;j++)
	{
		OledAddress(1+j,1);
		for(i=0;i<128;i++)
		{
			OledTransferData(0x00);
		}
	}
}

//*==============================================|=====================*
//*����ԭ��:
//*��������:	�������
//*��ڲ���:	<1=>����ϰ���
//*				<2=>����°���
//*==============================================|=====================*
void OledClearHalf(unsigned char part)
{
	unsigned char i,j;
	if(part!=1&&part!=2) return;
	
 	for(j=2*(part-1);j<2*part;j++)
	{
		OledAddress(1+j,1);
		for(i=0;i<128;i++)
		{
			OledTransferData(0x00);
		}
	}
}
//*==============================================|=====================*
//*����ԭ��:
//*��������:	���1/4��
//*��ڲ���:	<1=>��ֱ���0/4..1/4
//*				<2=>��ֱ���1/4..2/4
//*��ڲ���:	<3=>��ֱ���2/4..3/4
//*				<4=>��ֱ���3/4..4/4
//*==============================================|=====================*
void OledClearPart(unsigned char lr)
{
	unsigned char i,j;
	if(lr!=1&&lr!=2&&lr!=3&&lr!=4) return;
	
 	for(j=0;j<8;j++)
	{
		OledAddress(1+j,(lr-1)*32);
		for(i=32*(lr-1);i<=32*lr;i++)
		{
			OledTransferData(0x00);
		}
	}
}

//full display test
void oled_full_disp(uchar data1,uchar data2)
{
	int i,j;
	for(i=0;i<8;i++)
	{
		OledAddress(i+1,1);
		for(j=0;j<64;j++)
		{
			OledTransferData(data1);
			OledTransferData(data2);    			    
		}
	}          
}

//��ʾ128x32�����ͼƬ
void Display_128x32(uchar page,uchar column,uchar *dp)
{
	uint i,j;
	for(j=0;j<4;j++)
	{
		OledAddress(page+j,column);
		for (i=0;i<128;i++)
		{	
			OledTransferData(*dp);					//д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

//��ʾ����32x32����ͼ�񡢺��֡���Ƨ�ֻ�32x32���������ͼ��
void DisplayGraphic_32x32(uchar page,uchar column,uchar *dp)
{
	uchar i,j;
	for(j=0;j<4;j++)
	{
		OledAddress(page+j,column);
		for (i=0;i<32;i++)
		{	
			OledTransferData(*dp);		//д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

//��ʾ����16x16����ͼ�񡢺��֡���Ƨ�ֻ�16x16���������ͼ��
void DisplayGraphic_16x16(uchar page,uchar column,uchar *dp)
{
	uchar i,j;
	for(j=0;j<2;j++)
	{
		OledAddress(page-1,column);
		for (i=0;i<16;i++)
		{	
			OledTransferData(*dp);		//д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

//��ʾ����8x16����ͼ��ASCII, ��8x16����������ַ�������ͼ��
void DisplayGraphic_8x16(uchar page,uchar column,uchar *dp)
{
	uchar i,j;
	for(j=0;j<2;j++)
	{
		OledAddress(page+j,column);
		for (i=0;i<8;i++)
		{	
			OledTransferData(*dp);					//д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

//��ʾ8x16�ĵ�����ַ�����������Ĳ����ֱ�Ϊ��ҳ,�У��ַ���ָ�룩
void DisplayString_8x16(uint page,uint column,uchar text[])
{
	uint i=0,j,k,n;
	if(column>123)
	{
		column=1;
		page+=2;
	}
	while(text[i]>0x00)
	{	
		if((text[i]>=0x20)&&(text[i]<=0x7e))
		{
			j=text[i]-0x20;
			for(n=0;n<2;n++)
			{			
				OledAddress(page+n,column);
				for(k=0;k<8;k++)
				{					
					OledTransferData(ascii_table_8x16[j][k+8*n]);	//д���ݵ�LCD,ÿд��1�ֽڵ����ݺ��е�ַ�Զ���1
				}
			}
			i++;
			column+=8;
		}
		else
			i++;
	}
}


//д��һ��16x16����ĺ����ַ�����ȡģ�ĵ��������������躬�д��֣�
//������Ĳ�����(ҳ���У������ַ�����
void DisplayString_16x16(uchar page,uchar column,uchar *text)
{
    uchar i,j,k;
    uint address; 
    
    j = 0;
    while(text[j] != '\0')
    {
        i = 0;
        address = 1;
        while(Chinese_text_16x16[i] > 0x7e)	  // >0x7f��˵������ASCII���ַ�
        {
            if(Chinese_text_16x16[i] == text[j])
            {
                if(Chinese_text_16x16[i + 1] == text[j + 1])
                {
                    address = i * 16;
                    break;
                }
            }
            i += 2;            
        }
        
        if(column > 113)
        {
            column = 0;
            page += 2;
        }
        
        if(address != 1)// ��ʾ����                   
        {
            
			for(k=0;k<2;k++)
			{
				OledAddress(page+k,column);
	            for(i = 0; i < 16; i++)               
	            {
	                OledTransferData(Chinese_code_16x16[address]);   
	                address++;
	            }
   			}
            j += 2;        
        }
        else              //��ʾ�հ��ַ�            
        {
			for(k=0;k<2;k++)
			{
				OledAddress(page+k,column);
	            for(i = 0; i < 16; i++)               
	            {
	                OledTransferData(0x00);   
	            }
   			}
            
            j++;
        }
        
        column+=16;
    }
}
//��ʾ16x16����ĺ��ֻ���ASCII��8x16������ַ�����ַ���
//������Ĳ�����(ҳ���У��ַ�����
void DisplayString_8x16_16x16(uchar page,uchar column,uchar *text)
{
    uchar temp[3];
    uchar i = 0;    
    
    while(text[i] != '\0')
    {
        if(text[i] > 0x7e)
        {
            temp[0] = text[i];
            temp[1] = text[i + 1];
            temp[2] = '\0';          //����Ϊ�����ֽ�
            DisplayString_16x16(page,column,temp);  //��ʾ����
            column += 16;
            i += 2;
        }
        else
        {
            temp[0] = text[i];    
            temp[1] = '\0';          //��ĸռһ���ֽ�
            DisplayString_8x16(page, column, temp);  //��ʾ��ĸ
            column += 8;
            i++;
        }
    }
}

void OledDispString(uchar page,uchar column,uchar *text)
{
	DisplayString_8x16_16x16(page,column,text);
}

void OledPortInit( void )
{	
//WZ01_BR13_A��·����P52--SCL P53--SDA
	SetBit(P5M0,2);
	SetBit(P5M1,2);
	SetBit(P5M0,3);
	SetBit(P5M1,3);	
	
	OLED_SDA = 1;
	OLED_SCL = 1;
}


void OledSprintf(uchar page,uchar column,const char *format, ...)
{
	va_list args;
    va_start(args,format);
	memset(LineBuf,0,SIZEOF(LineBuf));
    vsprintf(LineBuf,format,args);
    va_end(args);
	
	OledDispString(page,column,LineBuf);	
}