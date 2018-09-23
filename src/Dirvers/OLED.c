/**====================================================================*
*							OLED(IIC 0.91寸)液晶驱动
*----------------------------------------------------------------------*
*文 件 名:    OLED.c
*文件描述:    0.91寸IIC接口OLED显示屏驱动程序
*软件版本:    V1.1(规范了函数命名规范)
*硬件环境:    WZ01_WN_A_V1.0
*创 建 人:    方规  526293582@qq.com(有限) 13477037750(手机)
*最后修改:    2017/06/08 16:51
*功能说明:    实现显示字符和汉字
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

//传8位指令或数据到OLED显示模块
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
    DelayUs(3);  //   ACK时间
    OLED_SCL=1; 
}

//写指令到OLED显示模块
void OledTransferCmd(uchar com)   
{
	OledStart(); 
	OledTransfer(0x78); 
	OledTransfer(0x00); 
	OledTransfer(com); 
	OledStop(); 
}

//写数据到OLED显示模块
void OledTransferData(uchar dat)
{
	OledStart(); 
	OledTransfer(0x78); 
	OledTransfer(0x40); 
	OledTransfer(dat); 
	OledStop(); 
} 


//OLED显示模块初始化
void OledInit()
{	
	OledPortInit();
	DelayMs(200); 
	
	OledTransferCmd(0xae);	//关显示
	
	OledTransferCmd(0xd5);	//晶振频率
	OledTransferCmd(0x80);
	
	OledTransferCmd(0xa8);	//duty设置
	OledTransferCmd(0x1f);	//duty=1/32
	
	OledTransferCmd(0xd3);	//显示偏移
	OledTransferCmd(0x00);	
	
	OledTransferCmd(0xa1);	//段重定向设置
	OledTransferCmd(0xa6);
	OledTransferCmd(0x40);	//起始行
	OledTransferCmd(0xb0);
	OledTransferCmd(0xd5);	
	OledTransferCmd(0xf0);
	
	
	OledTransferCmd(0x8d);	//升压允许
	OledTransferCmd(0x14);	
	
	OledTransferCmd(0x20);	//page address mode
	OledTransferCmd(0x02);
	
	OledTransferCmd(0xc8); //行扫描顺序：从上到下
	OledTransferCmd(0xa1); //列扫描顺序：从左到右
	
	OledTransferCmd(0xda); //sequential configuration
	OledTransferCmd(0x02);	//****************
	
	OledTransferCmd(0x81); //微调对比度,本指令的0x81不要改动，改下面的值
	OledTransferCmd(0xcf); //微调对比度的值，可设置范围0x00～0xff
	
	
	OledTransferCmd(0xd9); //Set Pre-Charge Period
	OledTransferCmd(0xf1);
	
	OledTransferCmd(0xdb); //Set VCOMH Deselect Level
	OledTransferCmd(0x49);
	
	OledTransferCmd(0xaf); //开显示
}

void OledAddress(uchar page,uchar column)
{
	column=column-1;  							//我们平常所说的第1列，在LCD驱动IC里是第0列。所以在这里减去1.
	page=page-1;
	OledTransferCmd(0xb0+page);   			    //设置页地址。每页是8行。一个画面的64行被分成8个页。我们平常所说的第1页，在LCD驱动IC里是第0页，所以在这里减去1
	OledTransferCmd(((column>>4)&0x0f)+0x10);	//设置列地址的高4位
	OledTransferCmd(column&0x0f);				//设置列地址的低4位
}

//全屏清屏
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
//*函数原型:
//*功能描述:	清除半屏
//*入口参数:	<1=>清除上半屏
//*				<2=>清除下半屏
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
//*函数原型:
//*功能描述:	清除1/4屏
//*入口参数:	<1=>垂直清除0/4..1/4
//*				<2=>垂直清除1/4..2/4
//*入口参数:	<3=>垂直清除2/4..3/4
//*				<4=>垂直清除3/4..4/4
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

//显示128x32点阵的图片
void Display_128x32(uchar page,uchar column,uchar *dp)
{
	uint i,j;
	for(j=0;j<4;j++)
	{
		OledAddress(page+j,column);
		for (i=0;i<128;i++)
		{	
			OledTransferData(*dp);					//写数据到LCD,每写完一个8位的数据后列地址自动加1
			dp++;
		}
	}
}

//显示单个32x32点阵图像、汉字、生僻字或32x32点阵的其他图标
void DisplayGraphic_32x32(uchar page,uchar column,uchar *dp)
{
	uchar i,j;
	for(j=0;j<4;j++)
	{
		OledAddress(page+j,column);
		for (i=0;i<32;i++)
		{	
			OledTransferData(*dp);		//写数据到LCD,每写完一个8位的数据后列地址自动加1
			dp++;
		}
	}
}

//显示单个16x16点阵图像、汉字、生僻字或16x16点阵的其他图标
void DisplayGraphic_16x16(uchar page,uchar column,uchar *dp)
{
	uchar i,j;
	for(j=0;j<2;j++)
	{
		OledAddress(page-1,column);
		for (i=0;i<16;i++)
		{	
			OledTransferData(*dp);		//写数据到LCD,每写完一个8位的数据后列地址自动加1
			dp++;
		}
	}
}

//显示单个8x16点阵图像、ASCII, 或8x16点阵的自造字符、其他图标
void DisplayGraphic_8x16(uchar page,uchar column,uchar *dp)
{
	uchar i,j;
	for(j=0;j<2;j++)
	{
		OledAddress(page+j,column);
		for (i=0;i<8;i++)
		{	
			OledTransferData(*dp);					//写数据到LCD,每写完一个8位的数据后列地址自动加1
			dp++;
		}
	}
}

//显示8x16的点阵的字符串，括号里的参数分别为（页,列，字符串指针）
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
					OledTransferData(ascii_table_8x16[j][k+8*n]);	//写数据到LCD,每写完1字节的数据后列地址自动加1
				}
			}
			i++;
			column+=8;
		}
		else
			i++;
	}
}


//写入一组16x16点阵的汉字字符串（取模的点阵数据数组中需含有此字）
//括号里的参数：(页，列，汉字字符串）
void DisplayString_16x16(uchar page,uchar column,uchar *text)
{
    uchar i,j,k;
    uint address; 
    
    j = 0;
    while(text[j] != '\0')
    {
        i = 0;
        address = 1;
        while(Chinese_text_16x16[i] > 0x7e)	  // >0x7f即说明不是ASCII码字符
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
        
        if(address != 1)// 显示汉字                   
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
        else              //显示空白字符            
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
//显示16x16点阵的汉字或者ASCII码8x16点阵的字符混合字符串
//括号里的参数：(页，列，字符串）
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
            temp[2] = '\0';          //汉字为两个字节
            DisplayString_16x16(page,column,temp);  //显示汉字
            column += 16;
            i += 2;
        }
        else
        {
            temp[0] = text[i];    
            temp[1] = '\0';          //字母占一个字节
            DisplayString_8x16(page, column, temp);  //显示字母
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
//WZ01_BR13_A电路板是P52--SCL P53--SDA
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