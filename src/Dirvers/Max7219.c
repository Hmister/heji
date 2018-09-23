/**====================================================================*
*						16*16点阵驱动及显示函数
*----------------------------------------------------------------------*
*文 件 名:    hal_max7219.c
*文件描述:    实现了MAX7219芯片的驱动以及16*16点阵显示的应用函数,用户直接
*			  调用即可简单的实现点阵的汉字/字符/数字等的显示
*软件版本:    V1.1
*硬件环境:    WZ-SN-V1.1  WZ-S02-V1.1
*创 建 人:    方规 526293582@qq.com 13477037750
*最后修改:    2017/06/07 10:19
*功能说明:    实现16*16点阵的字符串显示功能
-----------------------------------------------------------------------*
*
=======================================================================*/
#include "Max7219.h"

uchar LedDisp[32*4]=
{
	0xF8,0xF8,0x0C,0xC4, 0x0C,0xF8,0xF0,0x00, 0x03,0x07,0x0C,0x08, 0x0C,0x07,0x03,0x00,		//-0-		  	ASCII码：0X30
	0x00,0x10,0x18,0xFC, 0xFC,0x00,0x00,0x00, 0x00,0x08,0x08,0x0F, 0x0F,0x08,0x08,0x00,		//-1-
};
//将byte的第pos位取出来
uint8 gbit(uint8 byte,uint8 pos)
{
	return (1<<pos)&byte?1:0;
}
//将im[index]至im[index+7]各个字节的第pos位取出来,组成一个字节返回
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
//*函数原型:	
//*功能描述:	将列行式逆向转成行列式顺向
//*测    试:    已测试,方规
//*转 换 前:
//0x20,0x20,0x24,0x24,0x24,0xA4,0x24,0x24,
//0x20,0xFF,0x20,0x22,0x2C,0x20,0x20,0x00,
//0x40,0xC0,0x7E,0x40,0x40,0x3F,0x22,0x22,
//0x20,0x03,0x0C,0x10,0x20,0x40,0xF8,0x00,/*列行 逆向 "武",0*/
//*转 换 后:
//0x00,0x00,0x3F,0x00,0x00,0xFF,0x00,0x04,
//0x04,0x27,0x24,0x24,0x24,0x27,0xF8,0x40,
//0x40,0x50,0x48,0x48,0x40,0xFE,0x40,0x40,
//0x40,0x40,0x20,0x22,0x12,0x8A,0x06,0x02,/*行列 顺向 "武",0*/
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
//*函数原型:	
//*功能描述:	将列行式逆向转成行列式顺向
//*测    试:    已测试,方规
//*说    明:	00100000b -> 00000100b
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
//*函数原型:	
//*功能描述:	将列行式逆向转成行列式顺向
//*测    试:    已测试,方规
//*转 换 前:
//0x00,0x00,0xFC,0x00,0x00,0xFF,0x00,0x20,
//0x20,0xE4,0x24,0x24,0x24,0xE4,0x1F,0x02,
//0x02,0x0A,0x12,0x12,0x02,0x7F,0x02,0x02,
//0x02,0x02,0x04,0x44,0x48,0x51,0x60,0x40,/*行列 逆向 "武",0*/
//*转 换 后:
//0x00,0x00,0x3F,0x00,0x00,0xFF,0x00,0x04,
//0x04,0x27,0x24,0x24,0x24,0x27,0xF8,0x40,
//0x40,0x50,0x48,0x48,0x40,0xFE,0x40,0x40,
//0x40,0x40,0x20,0x22,0x12,0x8A,0x06,0x02,/*行列 顺向 "武",0*/
//*==============================================|=====================*
void NX2SX(uint8* imNX,uint8* exSX)
{
	uint8 i=0;
	for(i=0;i<32;i++){
		exSX[i] = Reverse(imNX[i]);
	}
}

//*==============================================|=====================*
//*函数原型:	
//*功能描述:	在OLED汉字库中查找汉字text的绝对位置
//*测    试:    已测试,方规
//*==============================================|=====================*
uint16 ChinesePos(uchar *text) //Chinese_code_16x16[ChinesePos("唯")];
{
	uint8 i = 0;
	uint16 address = 1;
	while(Chinese_text_16x16[i] > 0x7e)	  // >0x7f即说明不是ASCII码字符
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
//*函数原型:	
//*功能描述:	使用OLED字库中的字模来刷新LedDisp字模数据
//*测    试:    已测试,方规
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
//*函数原型:	
//*功能描述:	将8*16的字符字模,从列行式逆向转成行列式顺向
//*测    试:    已测试,方规
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
//*函数原型:	
//*功能描述:	完成对16*16汉字字模和8*16字符字模的转换(使用oled字库)
//*测    试:    已测试,方规
//*==============================================|=====================*
void CharUpdata(uchar* text)
{
	uint8 j=0;
	uint16 pos=0;	
	uchar* ptrAsc = (void *)ascii_table_8x16[0];
	if(text[0] > 0x7e){			//汉字	
		pos = ChinesePos(text);
		if(pos != -1){
			LHNX2HLSX((uchar*)Chinese_code_16x16+pos,LedDisp);
		}
	}else{						//ASCII字符	
		j=text[0]-0x20;			//得到字符在字库中的绝对位置
		memset(LedDisp,0,sizeof(LedDisp));
		CharModChange(ptrAsc+j*16,LedDisp);
		
		if(text[1] != '\0'){	//2个字符
			j=text[1]-0x20;
			CharModChange(ptrAsc+j*16,LedDisp+16);
		}
	}
}

//*==============================================|=====================*
//*函数原型:	
//*功能描述:	初始化IO口,用来控制MAX7219
//*测    试:    已测试,方规
//*==============================================|=====================*
void Init_MAX7219_Port(void)
{
//    P1SEL &= 0x70;            //让P00为普通IO口
//    P1DIR |= 0x07;            //设置为输出模式  
	P1M1 &= ~(3<<2);
	P1M0 |= (3<<2);
	P0M1 &= ~(1<<2);
	P0M0 |= (1<<2);
    
    Max7219_pinCS=1;
    Max7219_pinCLK=1;
    Max7219_pinDIN=1;
}
//*==============================================|=====================*
//*函数原型:	
//*功能描述:	初始化MAX7219
//*测    试:    已测试,方规
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
  DelayMs(time);					//显示暂停的时间
  Display_Max7219_Move(num,0);
}

//*==============================================|=====================*
//*函数原型:	
//*功能描述:	使用点阵显示特殊字符串,最多一次显示4个特殊字符,用于测试,直接在字模
//*				字串中手动添加字模
//*说    明:	需要将LedDisp改成[32*4]
//*测    试:    未测试
//*==============================================|=====================*
void fg_LedDisplay_4TS(uchar* hz)
{ 
	uint8 i=0;
	uint8 hzLen=strlen((char*)hz);
	if(hzLen>8) return;	
	for(i=0;i<hzLen/2;i++){
		Display_Dianzhen(4+i*4,300);//显示暂停时间,默认300ms		
	}
}


//*==============================================|=====================*
//*函数原型:	
//*功能描述:	使用点阵显示汉字字符串,最多一次显示4个汉字
//*说    明:	需要将LedDisp改成[32*4]
//*测    试:    已测试,方规
//*==============================================|=====================*
void fg_LedDisplay_4HZ(uchar* hz)
{ 
	uint8 i=0;
	uint8 hzLen=strlen((char*)hz);
	if(hzLen>8) return;	
	ChineseUpdate(hz);				//最多更新4个汉字
	for(i=0;i<hzLen/2;i++){
		Display_Dianzhen(4+i*4,300);//显示暂停时间,默认300ms		
	}
}



//*==============================================|=====================*
//*函数原型:	
//*功能描述:	使用点阵显示汉字字符串长度不限,只能用于显示字库中存在的汉字
//*				不适合显示ASCII字符
//*说    明:	需要将LedDisp改成[32*4]
//*测    试:    已测试,方规
//*==============================================|=====================*
void fg_LedDisplay_HZ(uchar* hz)
{ 
	uint8 len=0,j=0,i=0;
	uint8 temp[8];
	int hzLen=strlen((char*)hz);
	while(hzLen>0){
		len = strlen((char*)hz+j*8);	//8表示4个汉字的长度
		len = len>8?8:len%8;
		strncpy(temp,(char*)hz+j*8,len);		ChineseUpdate(temp);	
		
		for(i=0;i<len/2;i++){
			Display_Dianzhen(4+i*4,300);//显示暂停时间,默认300ms		
		}		
		
		j++;
		hzLen -= 8;
	}
}

//*==============================================|=====================*
//*函数原型:	
//*功能描述:	显示字符或汉字,最多显示2个字符(1个汉字),超过此长度将不能显示
//*				适合显示一屏
//*说    明:	需要将LedDisp改成[32*1]
//*测    试:    已测试,方规
//*==============================================|=====================*
void fg_LedDisplay_2ZF(uchar* zf)
{ 
	if(strlen(zf)>2) return;
	
	CharUpdata(zf);
	Display_Dianzhen(4,300);
}

//*==============================================|=====================*
//*函数原型:	
//*功能描述:	显示字符或汉字,可显示任意汉字字符组成的字符串
//*				适合显示多屏
//*说    明:	需要将LedDisp改成[32*4]
//*测    试:    已测试,方规
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
		
		Display_Dianzhen(4,300);//显示暂停时间,默认300ms						
		
		j++;
		hzLen -= 2;
	}
}