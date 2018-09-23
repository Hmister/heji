
#include <string.h>
#include "FG_Tool.h"

//*********************************************************************|********************
//*-----------------------------公用函数-------------------------------|-------------------*
//*********************************************************************|********************

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID30109134950 将字节流型的时间转换为U32型时间返回,方便传输
//*入口参数:	*imBytes--字节流型时间,格式:YYMMDDHHMMSS
//*返 回 值:	U32--U32型时间,格式:
//*             年B31..B21月B20..B17日B16..B12时B11..B6分B5..B0
//*说    明:	YY[0,2^11]
//*==============================================|=====================|===================*
uint32_t fg_TimeBytesReU32(uint8_t *imBytes)
{
	uint8_t i=0;
	uint32_t u32Time=0;
	for(i=0;i<5;i++) imBytes[i] = fg_HexReDec(imBytes[i]);
	fg_SetBitsInU32(7,21,imBytes[0],&u32Time);	//F_:YY	 0x14 0x02800000      
	fg_SetBitsInU32(4,17,imBytes[1],&u32Time);	//F_:MM	 0x01 0x02820000      
	fg_SetBitsInU32(5,12,imBytes[2],&u32Time);	//F_:DD	 10	  0x02843000      
	fg_SetBitsInU32(6, 6,imBytes[3],&u32Time);	//F_:HH	 10
	fg_SetBitsInU32(6, 0,imBytes[4],&u32Time);	//F_:MM	 30
	return u32Time;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID30109142907 将U32型时间转换为字节流行返回,方便观看
//*入口参数:	imU32--U32型时间
//*返 回 值:	static*--字节流型时间,格式:YYMMDDHHMMSS
//*==============================================|=====================|===================*
uint8_t* fg_TimeU32ReBytes(uint32_t imU32)
{
	static uint8_t times[5];
	times[0] = fg_GetBitsInU32(7,21,imU32);	//F_:YY
	times[1] = fg_GetBitsInU32(4,17,imU32);	//F_:MM
	times[2] = fg_GetBitsInU32(5,12,imU32);	//F_:DD
	times[3] = fg_GetBitsInU32(6, 6,imU32);	//F_:HH
	times[4] = fg_GetBitsInU32(6, 0,imU32); //F_:MM
	return times;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID30110145313 将整数转换为4元素数组
//*入口参数:	imU32--输入的整数,最长为U32型
//*返 回 值:	static*--转换后的数组指针
//*应用示例:	im:0x12345678 -> re:{0x12,0x34,0x56,0x78}
//*             im:0x1234 -> re:{0x00,0x00,0x12,0x34}
//*==============================================|=====================|===================*
uint8_t* fg_U32ReBytes(uint8_t imU32)
{
	static uint8_t reBytes[4];
	uint8_t i=0;
	for(i=0;i<4;i++) reBytes[i] = imU32 >> (3-i)*8;
	return reBytes;	
}

//*==============================================|=====================|===================*
//两个字节进行半字节交换
//入口：*Byte1 *Byte2 待交换的两个字节
//		Byte1HL:1 第一个字节的高半字节 0 第一个字节的低半字节
//		Byte2HL:1 第二个字节的高半字节 0 第二个字节的低半字节
//出口：*Byte1 *Byte2 交换后的两个字节
//*==============================================|=====================|===================*
void zk_2BytesXhg(uint8_t *Byte1,uint8_t *Byte2,uint8_t Byte1HL,uint8_t Byte2HL)
{
	uint8_t TemA,TemB,Temp;
	TemA = *Byte1;
	TemB = *Byte2;
	if(!Byte1HL)
	{
		Temp = TemA & 0x0F;
		if(!Byte2HL)
		{
			TemA = (TemA&0xF0)|(TemB&0x0F);
			TemB = (TemB&0xF0)|Temp;
		}
		else
		{
			TemA = (TemA&0xF0)|(TemB>>4);
			TemB = (TemB&0x0F)|(Temp<<4);
		}
	}
	else
	{
		Temp = TemA & 0xF0;
		if(!Byte2HL)
		{
			TemA = (TemA&0x0F)|(TemB<<4);
			TemB = (TemB&0xF0)|(Temp>>4);
		}
		else
		{
			TemA = (TemA&0x0F)|(TemB&0xF0);
			TemB = (TemB&0x0F)|Temp;
		}
	}
	*Byte1 = TemA;
	*Byte2 = TemB;
}

/********************************************************************************************
*函数名称:cp_RLBuf
*函数功能:循环左移MoveTimes次
*入口参数:TemDataBuf（数据）,BufLen(缓冲区长度),MoveTimes(移动次数)
*出口参数:TemDataBuf
*返回结果:无
********************************************************************************************/
void cp_RLBuf(unsigned char *TemDataBuf,unsigned char BufLen,unsigned char MoveTimes)
{
	unsigned char i,j;
	unsigned char uchar_TemV,uchar_LastBit;
	unsigned char *uchar_p;
	
	for (i=0;i<MoveTimes;i++)
	{
		uchar_p=TemDataBuf;
		uchar_LastBit=(*uchar_p & 0x80);                      //取得第一字节的最高位
		uchar_LastBit>>=7;                                     //放到最低位，准备装入到最后一字节的最后一位
		for (j=0;j<BufLen;j++)
		{
			if (j==BufLen-1)                                   
			{                                                 //最后一字节则或上LastBit
				*uchar_p<<=1;
				*uchar_p=*uchar_p | uchar_LastBit; 
			}
			else
			{ 
				uchar_TemV=(*(uchar_p+1) & 0x80);             //取得下一字节的最高位
				uchar_TemV>>=7;                                //放到最高低位，准备装入到上一字节
				*uchar_p<<=1;                                  //左移1位
				*uchar_p=*uchar_p | uchar_TemV;               //或上下一字节的最高位作为最低位
				uchar_p++;
			}
		} 
	}
}
/********************************************************************************************
*函数名称:cp_RRBuf
*函数功能:循环右移MoveTimes次
*入口参数:TemDataBuf（数据）,BufLen(缓冲区长度),MoveTimes(移动次数)
*出口参数:TemDataBuf
*返回结果:无
********************************************************************************************/
void cp_RRBuf(unsigned char *TemDataBuf,unsigned char BufLen,unsigned char MoveTimes)
{
	unsigned char i,j;
	unsigned char uchar_TemV,uchar_LastBit;
	unsigned char *uchar_p;
	
	for (i=0;i<MoveTimes;i++)
	{
		uchar_p=TemDataBuf;
		uchar_LastBit=(*(uchar_p+BufLen-1) & 0x01);    	//取得最后一字节的最低位
		uchar_LastBit<<=7;                             	//放到最高位，准备装入到第一字节
		for(j=0;j<BufLen;j++)
		{
			uchar_TemV=(*uchar_p & 0x01);              	//取得最后一字节的最低位
			uchar_TemV<<=7;                             //放到最高位，准备装入到第一字节
			*uchar_p>>=1;                               //右移1位
			*uchar_p=*uchar_p | uchar_LastBit;         	//或上上一字节的最低位作为最高位
			uchar_p++;
			uchar_LastBit=uchar_TemV;
		} 
	}
}


//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31219221126 检查字节流中指定数据的个数
//*入口参数:	imLen--字节流长度
//*             *imBytes--字节流指针
//*             imTarget--指定的数据
//*返 回 值:	result--指定数字的个数,与imLen相等表示字节流中数据全为target
//*==============================================|=====================|===================*
uint8_t fg_CheckBytes(uint8_t imLen,uint8_t *imBytes,uint8_t imTarget)
{
	uint8_t i=0,result=0;
	for(i=0;i<imLen;i++) 
	{
		if(*(imBytes+i)==imTarget) result++;
	}
	return result;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31112120722 初始化字节流
//*入口参数:	imLen--字节流长度
//*             *imPtr--字节流指针
//*             imValue--初始化的值
//*==============================================|=====================|===================*
void fg_InitBytes(uint8_t imLen,uint8_t *imPtr,uint8_t imValue)
{
	uint8_t i=0;
	for(i=0;i<imLen;i++) *(imPtr+i) = imValue;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31112102623 复制N个字节
//*入口参数:	imLen--N的大小
//*             *imArray--被复制的源数据
//*出口参数:	*exArray--复制之后的数据
//*应用示例:	im:0x00 0x01 0x02 0x03 -> ex:0x00 0x01 0x02 0x03 Len is 4 
//*==============================================|=====================|===================*
void fg_CopyBytes(uint8_t imLen,uint8_t* imArray,uint8_t* exArray)
{
	uint8_t i = 0;
	for(i=0;i<imLen;i++)
		*(exArray+i) = *(imArray+i);	
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31112102908 倒序复制N个字节,imH->exL
//*入口参数:	imLen--N的大小
//*             *imArray--被复制的源数据
//*出口参数:	*exArray--复制之后的数据
//*应用示例:	im:0x00 0x01 0x02 0x03 -> ex:0x03 0x02 0x01 0x00 Len is 4 
//*             im:0x00 0x01 0x02 0x03 -> ex:0x02 0x01 0x00 Len is 3
//*==============================================|=====================|===================*
void fg_CopyBytes_H2L(uint8_t imLen,uint8_t* imArray,uint8_t* exArray)
{
	uint8_t i = 0;
	for(i=0;i<imLen;i++)
		*(exArray+i) = *(imArray+imLen-i-1);	
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31112120343 比较字节流(字符串,数组,结构体...)
//*入口参数:	imLen--字节流长度
//*             *str1--字节流1
//*             *str2--字节流2
//*返 回 值:	<0=>相等
//*             <!0=>不相等的字节
//*应用示例:	str1:0x01 0x02 0x03 0x04 <-> str2:0x04 0x03 0x02 0x01 Len is 4 return 1
//*             str1:0x01 0x02 0x03 0x04 <-> str2:0x01 0x02 0x03 0x04 Len is 4 return 0
//*==============================================|=====================|===================*
uint8_t fg_BytesCompare(uint8_t imLen,uint8_t *str1,uint8_t *str2)
{
	uint8_t i = 0;
	for(i=0;i<imLen;i++)
		if(*(str1+i) != *(str2+i))
			return 1+i;
	return 0;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31112115545 倒序比较字节流(字符串,数组,结构体...)
//*入口参数:	imLen--字节流长度
//*             *str1--字节流1
//*             *str2--字节流2
//*返 回 值:	<0=>相等
//*             <!0=>不相等的字节
//*应用示例:	str1:0x01 0x02 0x03 0x04 <-> str2:0x04 0x03 0x02 0x01 Len is 4 return 0
//*             str1:0x01 0x02 0x03 0x04 <-> str2:0x01 0x02 0x03 0x04 Len is 4 return 1
//*==============================================|=====================|===================*
uint8_t fg_BytesCompare_L2H(uint8_t imLen,uint8_t *str1,uint8_t *str2)
{
	uint8_t i = 0;
	for(i=0;i<imLen;i++)
		if(*(str1+i) != *(str2+imLen-i-1))
			return 1+i;
	return 0;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID3110801408 获取二进制数中的第一个0(从右往左)
//*入口参数:	imNum--需要操作的数据
//*返 回 值:	<0..31=>从右往左的第一个0的位置
//*             <0xFF=>入口数据全1
//*测    试:    20131126,方规,已测试
//*应用示例:	imNum=8493104 -> return 0
//*==============================================|=====================|===================*
uint8_t fg_GetFirst0ByBin(uint32_t imNum)
{
	uint8_t i = 0;
	for(i=0;i<32;i++)
		if(((imNum >> i)&1) == 0)				//F_:位逻辑运算优先级低!!!
			return i;
	return 0xFF;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31126112518 从右往左的第一个1的位置
//*入口参数:	imNum--目标数据
//*返 回 值:	<0xFF=>全0
//*             <n=>结果
//*测    试:    20131126,方规,已测试
//*应用示例:	imNum=8493104 -> return 4
//*==============================================|=====================|===================*
uint8_t fg_GetFirst1ByBin(uint32_t imNum)
{
	uint8_t i=0;
	for(i=0;i<32;i++)
		if(((imNum >> i)&1) == 1)				//F_:位逻辑运算优先级低!!!
			return i;
	return 0xFF;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31126160509 将U32数据的某一位置1
//*入口参数:	im1--指定某位
//*双向参数:	imNum--需要设置的数据
//*返 回 值:	操作结果
//*测    试:    20131126,方规,已测试
//*应用示例:	im(3,0) -> re(8)
//*说    明:	此函数不改变实参的imNum的值,结果由返回值暂存
//*测    试:    20131206,方规,已测试
//*==============================================|=====================|===================*
uint32_t fg_SetOneBit(uint8_t im1,uint32_t imNum)
{
	return imNum |= (1<<im1);
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31126160441 将U32数据的某一位清0
//*入口参数:	im1--指定某位
//*双向参数:	imNum--需要清零的数据
//*返 回 值:	操作结果
//*测    试:    20131126,方规,已测试
//*应用示例:	im(3,255) -> re(247)  
//*说    明:	此函数不改变实参的imNum的值,结果由返回值暂存
//*测    试:    20131206,方规,已测试
//*==============================================|=====================|===================*
uint32_t fg_ClrOneBit(uint8_t im1,uint32_t imNum)
{
	return imNum &= ~(1<<im1);
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31206111425 获取U32型数据中的指定位的值
//*入口参数: 	imBitLen--需要获取的位长度
//*             imOffset--首位的偏移(相对于第0位)
//*             imNum--U32型目标数据
//*返 回 值:	获取的指定位的值,Value=[0,imNum]
//*应用示例:	im:5,1,0x12345678 -> re:11100b=0x1C
//*测    试:    20131206,方规,已测试
//*==============================================|=====================|===================*
uint32_t fg_GetBitsInU32(uint8_t imBitLen,uint8_t imOffset,uint32_t imNum)
{		  
	return ((imNum>>imOffset) & ((1<<imBitLen)-1));	
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31206143027 设置U32型数据中指定位数的值
//*入口参数: 	imBitLen--需要获取的位长度
//*             imOffset--首位的偏移(相对于第0位)
//*             imBitsValue--需要设置的指定位的值 
//*双向参数:	*ieNum--U32目标操作数
//*返 回 值:	结果
//*应用示例:	im:5,1,0,num(0x12345678) -> re:0x12345640 ex:num(0x12345640)
//*应用示例:	im:1,16,1,num(0x0000000) -> re:0x00010000 ex:num(0x00010000)
//*             im:1,16,0,num(0xFFFFFFF) -> re:0xFFFEFFFF ex:num(0xFFFEFFFF)
//*说    明:	"<<"的优先级低于"+"
//*测    试:    20131212,方规,已测试
//*==============================================|=====================|===================*
uint32_t fg_SetBitsInU32(uint8_t imBitLen,uint8_t imOffset,uint32_t imBitsValue,uint32_t *ieNum)
{
	return *ieNum = (*ieNum&(-1<<imOffset+imBitLen))+
					(imBitsValue<<imOffset)+(*ieNum&((1<<imOffset)-1));
}						

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31127132536 从字节流中截取最多4个字节,转换成uint32_t型返回
//*入口参数:	imTruLen--截取长度,最大为4对应uint32_t
//*             *imBytes--字节流指针
//*返 回 值:	result--结果
//*说    明:	u32_--函数的返回值为uint32_t型
//*             LB--低地值存储大值(大端)
//*应用示例:	0x01 0x02 0x03 0x04 -> 0x01020304	Len is 4
//*说    明:	使用前得保证imOffset不能超过字节流的长度
//*==============================================|=====================|===================*
uint32_t fg_BytesTruncByU32(uint8_t imTruLen,uint8_t *imBytes)
{
	uint8_t i = 0;
	uint32_t result = 0;
	for(i=0;i<imTruLen;i++)
	{
		result |= (*(imBytes+i) << (imTruLen-i-1)*8);
	}
//*小端模式
//	for(i=0;i<imTruLen;i++)
//	{
//		result |= (*(imBytes+imOffset+i) << i*8);
//	}
	return result;	
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31127145311 将一个U32型数据按BL(高字节在前)粘贴至字节流中,不足4丢失大数(DB)
//*入口参数:	imPasLen--粘贴的长度,最大4字节
//*             *imBytes--字节流
//*             imU32Num--U32型数据
//*应用示例:	im:0x12345678 -> ex:--34--56--78-- 	len:3
//*             im:0x12345678 -> ex:--56--78--		len:2		
//*说    明:	当粘贴的数据不足4个字节的时候,将丢失U32中的"大数"		
//*==============================================|=====================|===================*
void fg_U32PasteInBytes_DB(uint8_t imPasLen,uint32_t imU32Num,uint8_t *ieBytes)
{
	uint8_t i = 0;
	for(i=0;i<imPasLen;i++)
		*(ieBytes+imPasLen-1-i) = *((uint8_t*)&imU32Num+i);	
}					

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31204164510 将一个U32型数据按BL(高字节在前)粘贴至字节流中,不足4丢失小数(DS)
//*入口参数:	imPasLen--粘贴的长度,最大4字节
//*             *imBytes--字节流
//*             imU32Num--U32型数据
//*应用示例:	im:0x12345678 -> ex:--12--34--56-- 	len:3
//*             im:0x12345678 -> ex:--12--34--		len:2
//*说    明:	当粘贴的数据不足4个字节的时候,将丢失U32中的"小数"		
//*==============================================|=====================|===================*
void fg_U32PasteInBytes_DS(uint8_t imPasLen,uint32_t imU32Num,uint8_t *ieBytes)
{
	uint8_t i = 0;
	for(i=0;i<imPasLen;i++)
		*(ieBytes+i) = *((uint8_t*)&imU32Num+3-i);	
}

//*==============================================|=====================|===================*
//*函数原型:    uint8_t MakeXor(uint8_t *imData,uint8_t imB,uint8_t imE);
//*编号功能:    CID30910160236 求异或和
//*入口参数:	*imData--需要求和的数据指针
//*             imB--第一个数据
//*             imE--最后一个数据
//*返 回 值:	result--结果
//*应用示例:    buf_comm[14] = MakeXor(buf_comm,2,13);
//*==============================================|=====================|===================*
uint8_t fg_MakeXor(uint8_t *imData,uint8_t imB,uint8_t imE)
{
	uint8_t result = 0;
	uint8_t i = 0;
	for(i=imB;i<=imE;i++)
	{
		result ^= *(imData+i);		
	}
	return result;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*功能描述:    将多个单字节数据组合成字或者双字类型的数据
//*入口参数:	imCLength--字节个数
//*		*imCData--字节数据指针
//*返 回 值:	lRev--多个字节组合成双字型数据
//*说    明:	组合方式是高字节在前
//*应用示例:	0x01 0x02 0x04 0x08 -> 0x01020408	length is 4
//*测    试:    20131102,方规,已测试
//*==============================================|=====================|===================*
uint32_t fg_BytesToLong_L2B(uint8_t imCLength,uint8_t* imCData)		//F_:export 出口 import入口	
{													
	uint8_t i=0; 
	uint32_t lRev=0;
	lRev = *(imCData+imCLength-1);
	for(i=0;i<imCLength;i++)
	{
		lRev |= *(imCData+i) << (imCLength-i-1)*8;	
	}	
	return lRev;

}
//*==============================================|=====================|===================*
//*函数原型:        
//*功能描述:    将一个长整形的数据按照高字节在前拆分成多个字节
//*入口参数:    imCLength--才分字节的个数
//*             imLData--需要拆分的长整形数
//*返 回 值:	*pCRev--拆分之后的字节数据指针,高字节在前
//*应用示例:    0x12345678 -> {0x12,0x34,0x56,0x78}    length is 4
//*             0x12345678 -> {0x56,0x78}	length is 2	   
//*==============================================|=====================|===================*
void fg_LongToBytes_B2L(uint8_t imCLength,uint32_t imLData,uint8_t* exPData)
{
    int8_t i=0;
    for(i=imCLength-1;i>=0;i--)				  //F_: i=1;i>=0;i++
    {
        *( exPData+i) = (imLData & (0xFF << (imCLength-1-i)*8)) >> (imCLength-1-i)*8;
    }
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31025164705 将HEX数组数据转换成字符串
//*入口参数:	imLen--数组长度
//*             *imHex--HEX数组指针
//*出口参数:	*exStr--转换之后的指针
//*应用示例:	0x12 0xA2 0x32 0x0D -> "12A2320D" length is 4
//*说    明:	exStr应该用数组来接收,其长度应该是imLen*2+1(包含'\0')
//*==============================================|=====================|===================*
void Hex2Str(uint8_t imLen,uint8_t *imHex,uint8_t *exStr)
{
	uint8_t i = 0;
	uint8_t tempH,tempL;

	for(i=0;i<imLen;i++)
	{
		tempH = imHex[i] >> 4;
		tempL = imHex[i] & 0x0F;
	    exStr[i*2] = (tempH > 9) ? (tempH - 10 + 'A') : (tempH + '0');
		exStr[i*2+1] = (tempL > 9) ? (tempL - 10 + 'A') : (tempL + '0');
	}
	*(exStr+imLen*2) = '\0';
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31115212822 连接字节流
//*入口参数:	imBytesLen--被连接的字节流长度
//*             imAddLen--需要增加的长度
//*             *imBytes--连接的字节流指针
//*双向参数:	*ieBytes--被连接字节流指针
//*测    试:    20131115,方规,已测试
//*应用示例:	fg_BytesAddBytes(4,4,&buf,"fang") -> buf:"xxxxfangg"
//*==============================================|=====================|===================*
uint8_t* fg_BytesAddBytes(uint8_t imBytesLen,uint8_t imAddLen,uint8_t *ieBytes,uint8_t *imBytes)
{
	uint8_t i = 0;
	for(i=0;i<imAddLen;i++)
		*(ieBytes+imBytesLen+i) = *(imBytes+i);
	return ieBytes;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31115211907 将数字后面的四位数以ASC方式显示为错误码并加"Err:"
//*入口参数:	imNum--数字
//*出口参数:	exPtr--错误码字串指针
//*测    试:    20131115,方规,已测试 两种字节流连接方法均可!!
//*应用示例:	fg_GetErrCodePtr(0x12345678,&buf) -> buf: "Err:5678"
//*==============================================|=====================|===================*
uint8_t* fg_GetErrCodePtr(uint32_t imNum,uint8_t *exPtr)
{
	uint8_t temp[2]={0};
	uint8_t temp2[4];
	fg_CopyBytes(4,"Err:",exPtr);
	fg_CopyBytes_H2L(2,(uint8_t*)&imNum,temp);
	Hex2Str(2,temp,temp2);
//	return exPtr = (uint8_t*)strcat((char*)exPtr,(const char*)temp2);
	return fg_BytesAddBytes(4,4,exPtr,temp2);
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31121152054 从字节流中移除一个指定的字符,值判定
//*入口参数:	imLen--字节流长度
//*           	imValue--需要移除的值
//*==============================================|=====================|===================*
uint8_t fg_RemoveOneByValue(uint8_t imLen,uint8_t *imBytes,uint8_t imValue)
{
	uint8_t i =0 ;
	uint8_t targetPos = 0;
	for(i=0;i<imLen;i++)
	{
		if(*(imBytes+i)==imValue)
			break;
	}
	targetPos = i;
	for(i=targetPos;i<imLen;i++)
	{
		*(imBytes+i) = *(imBytes+i+1);
	}
	return targetPos;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31125111149 BCD转U32
//*入口参数:	imBcdLen--BCD长度
//*             imLongLen--U32有效字节数
//*             imSort--HEX输出的循序 <0=>L2B(高字节在前) <1=>L2S(低字节在前)
//*             *imBcd--需要转换的BCD数据指针
//*返 回 值:	long--BCD转换之后的结果
//*应用示例:	Bcd={0x12,0x34,0x56,0x78} -> U32=0xBC614E BcdLen=4,HexLen=3
//*             							 U32=0x00BC614E BcdLen=4,HexLen=4
//*测    试:    20131204,方规,已测试
//*==============================================|=====================|===================*
uint32_t fg_BcdToLong(uint8_t imBcdLen,uint8_t imLongLen,uint8_t imSort,uint8_t *imBcd)
{
	uint8_t hex[4]={0};
	fg_BcdToHex(imBcdLen,imLongLen,imSort,imBcd,hex);
	return fg_BytesToLong_L2B(imLongLen,hex);
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31122102036 BCD转HEX
//*入口参数:	imBcdLen--BCD长度
//*             imHexLen--HEX输出长度
//*             imSort--HEX输出的循序 <0=>L2B(高字节在前) <1=>L2S(低字节在前)
//*             *imBcd--需要转换的BCD数据指针
//*出口参数:	*exHex--转换之后的HEX数据指针
//*应用示例:	Bcd:{0x12,0x34,0x56,0x78} -> Hex:{0xBC,0x61,0x4E} BcdLen=4,HexLen=3 
//*             							 Hex:{0x00,0xBC,0x61,0x4E} BcdLen=4,HexLen=4
//*==============================================|=====================|===================*
void fg_BcdToHex(uint8_t imBcdLen,uint8_t imHexLen,uint8_t imSort,uint8_t *imBcd,uint8_t *exHex)
{
	uint8_t  tempBuf[8]={0};    
	uint8_t  i,CurBCDPos;
	uint16_t TemInt,TemADDC;
	
	//先按最长的llong型处理成8字节数据,低位在前
	for(CurBCDPos=0;CurBCDPos<imBcdLen;CurBCDPos++)
	{
		TemADDC=0;                              //TemADDC保存进位
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]*10;         		//*10
			TemInt+=TemADDC;                    //加上进位
			tempBuf[i]=TemInt % 256;      		//保存本位的值
			TemADDC=TemInt / 256;               //得到进位
		}
		TemInt=*(imBcd+CurBCDPos);         		//加上本字节的高位 
		TemInt&=0xf0;
		TemInt>>=4;
		TemADDC=TemInt;
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]+TemADDC;   		//增加到HEX缓冲区
			tempBuf[i]=TemInt % 256;
			TemADDC=TemInt / 256;
		}
		TemADDC=0;                              //TemADDC保存进位
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]*10;         		//*10
			TemInt+=TemADDC;                    //加上进位
			tempBuf[i]=TemInt % 256;      		//保存本位的值
			TemADDC=TemInt / 256;               //得到进位
		}
		TemInt=*(imBcd+CurBCDPos);         		//加上本字节的低位 
		TemInt&=0x0f;
		TemADDC=TemInt;
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]+TemADDC;   		//增加到HEX缓冲区
			tempBuf[i]=TemInt % 256;
			TemADDC=TemInt / 256;
		}
	}
	//输出,HEX是低位在前
	if(imSort)	for(i=0;i<imHexLen;i++)	*(exHex+i)=tempBuf[i];
	//输出,HEX高位在前
	else for(i=0;i<imHexLen;i++) *(exHex+i)=tempBuf[imHexLen-i-1];
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31122142910 两个BCD数相加
//*入口参数:	imByte1--被加数
//*             imByte2--加数
//*             imCarryFlag--上次运算的进位标志,调用一次不用参数
//*出口参数:	*exBytes--结果
//*==============================================|=====================|===================*
uint8_t fg_BcdAdd(uint8_t imByte1,uint8_t imByte2,uint8_t imCarryFlag,uint8_t *exBytes)
{
	uint8_t uchar_TemL1,uchar_TemH1,uchar_TemL2,uchar_TemH2,TemChar;
	uint8_t uchar_ADDCF=0;
	
	//先分解成各高低位
	uchar_TemL1=imByte1 & 0x0f;
	uchar_TemH1=imByte1 & 0xf0;
	uchar_TemH1>>=4;
	uchar_TemL2=imByte2 & 0x0f;   
	uchar_TemH2=imByte2 & 0xf0;
	uchar_TemH2>>=4;
	
	//低位相加
	TemChar=uchar_TemL1+uchar_TemL2+imCarryFlag;
	//判断是否有AC进位或>9,如果是则低4位加6（相当于DA调整）
	if(TemChar>9) TemChar=TemChar+6;
	uchar_TemL1=TemChar & 0x0f;
	//高位相加
	TemChar>>=4;
	TemChar=uchar_TemH1+uchar_TemH2+TemChar;
	//判断是否有C进位或>9,如果是则高4位加6（相当于DA调整）
	if(TemChar>9) TemChar=TemChar+6;
	uchar_TemH1=TemChar;
	uchar_TemH1<<=4;
	//取得下一进位作为输出返回
	TemChar>>=4;
	uchar_ADDCF = TemChar?1:0;
	//得到BCD码和
	*exBytes=uchar_TemH1 | uchar_TemL1;
	
	return uchar_ADDCF;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31122133818 U32整形数据转换为BCD
//*入口参数:	imU32Num--需要转换的U32型数据
//*             imBcdOutLen--指定输出BCD的长度,最多5字节(U32型的数最多转换为5字节的BCD)
//*             imSort--指定BCD输出的字节序 <0=>_BS(从大到小) <1=>_SB(从小到大)
//*出口参数:	*exBcdBuf--输出的BCD指针
//*==============================================|=====================|===================*
void fg_LongToBcd(uint64_t imU32Num,uint8_t imBcdOutLen,uint8_t imSort,uint8_t *exBcdBuf)
{
	uint64_t tempOut=0;
	uint8_t *ptrTemp=NULL;   
	uint8_t i,j,tempLen;
	uint8_t *sourBuf=(uint8_t *)&tempOut;   //要转换的LLONG型数据的指针
	uint8_t outBitTemp=0;
	uint8_t tempBuf[8]={0};

	//初始化
	tempOut=imU32Num;
	for(i=0;i<64;i++)
	{    
		//循环左移，取得位数
		outBitTemp = *(sourBuf+7)&0x80 ? 1:0;
		tempOut<<=1;
		for(j=0;j<8;j++) 
		outBitTemp=fg_BcdAdd(tempBuf[j],tempBuf[j],outBitTemp,&tempBuf[j]);
	}
	
	//最大长度只能是8
	tempLen=imBcdOutLen;
	if(tempLen>8) tempLen=8;
	ptrTemp=exBcdBuf;
	//准备输出
	for(i=0;i<tempLen;i++,ptrTemp++)
	{
		if(!imSort)	*ptrTemp=tempBuf[tempLen-i-1];	//F_:从大到小
		else		*ptrTemp=tempBuf[i];			//F_:从小到大
	}
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31122140452 将HEX数据转换成BCD数据
//*入口参数:	imHexLen--HEX数据长度
//*             imBcdLen--转换之后BCD数据长度(最多比imHexLen大1)
//*             imSort--BCD输出字节序 <0=>_BS(从大到小) <1=>_SB(从小到大)
//*             *imHex--HEX数据指针
//*出口参数:	*exBcd--转换之后BCD数据指针	
//*==============================================|=====================|===================*
void fg_HexToBcd(uint8_t imHexLen,uint8_t imBcdLen,uint8_t imSort,uint8_t *imHex,uint8_t *exBcd)
{
	uint64_t u64Temp=0;
	fg_CopyBytes_H2L(imHexLen,imHex,(uint8_t*)&u64Temp);
	fg_LongToBcd(u64Temp,imBcdLen,imSort,exBcd);
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID30101150427 将一个字节的HEX数转换成DEC
//*入口参数:	imHex--HEX数
//*返 回 值:	reDec--DEC数
//*应用示例:	im:0x14 -> re:14
//*==============================================|=====================|===================*
uint8_t fg_HexReDec(uint8_t imHex)
{
	return (imHex/16)*10+imHex%16;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31220161801 将HEX数组转换成U32型数值,最多4字节
//*入口参数:	imLen--指定HEX数组长度
//*				*imHex--HEX数组指针
//*返 回 值:	number--转换之后的数值
//*应用示例:
//*==============================================|=====================|===================*
uint32_t fg_HexReNumber(uint8_t imLen,uint8_t *imHex)
{
	uint8_t  i = 0;
	uint8_t  tempH,tempL;
	uint32_t num = 0;

	for(i=0;i<imLen;i++)
	{
		tempH = imHex[i] >> 4;
		tempL = imHex[i] & 0x0F;
		if(tempH > 9 || tempL > 9) return 0;

		num = num*100 + tempH*10;
		num = num + tempL;
	}
	return num;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31221173306 将带浮点的字符串,转换成整形,指定处理小数点位数
//*入口参数:	*imStr--字串指针
//*             imDig--指定有效处理的小数点位数,通常是2位,2位之后的将不予处理
//*返 回 值:	result--处理之后的结果
//*说    明:	imStr应该要保证,最多只能含有一个小数点
//*应用示例:	im:"1234.5678",2 -> re:123456
//*             im:"123456",2 -> re:12345600
//*==============================================|=====================|===================*
uint32_t fg_InputStrToNum(uint8_t *imStr,uint8_t imDigit)
{
	uint8_t i=0;
	uint8_t dotPos=0;	   					//F_:小数点的位置,如果等于strlen(imStr)表示无小数点
	uint8_t floatCnt=0;						//F_:小数的位数
	uint32_t result=0;						//F_:整数部分的结果
	uint32_t floatValue=0;					//F_:小数部分的结果

	for(i=0;imStr[i]&&imStr[i]!='.';i++) 
		dotPos = i+1;						//F_:小数点在字串中的下标
	for(i=0;i<dotPos&&imStr[i];i++)
	{
		result *= 10;
		result += imStr[i]-'0';
	}
	for(i=0;i<imDigit;i++,result*=10);		//F_:扩大10^imDigit倍
	floatCnt = strlen(imStr)-dotPos-1;		//F_:小数的位数
	for(i=0;floatCnt&&i<imDigit;i++)		//F_:计算小数部分的值
	{										
		if(imStr[dotPos+1+i]) 
			floatValue += imStr[dotPos+1+i]-'0';
		if(i==imDigit-1) break;
		floatValue *= 10;
	}

	return result+floatValue;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31025164705 将HEX数组数据转换成字符串
//*入口参数:	imLen--数组长度
//*             *imHex--HEX数组指针
//*出口参数:	*exStr--转换之后的指针
//*应用示例:	0x12 0xA2 0x32 0x0D -> "12A2320D" length is 4
//*说    明:	exStr应该用数组来接收,其长度应该是imLen*2+1(包含'\0')
//*==============================================|=====================|===================*
void fg_HexToString(uint8_t imLen,uint8_t *imHex,uint8_t *exStr)
{
	uint8_t i = 0;
	uint8_t tempH,tempL;

	for(i=0;i<imLen;i++)
	{
		tempH = imHex[i] >> 4;
		tempL = imHex[i] & 0x0F;
	    exStr[i*2] = (tempH > 9) ? (tempH - 10 + 'A') : (tempH + '0');
		exStr[i*2+1] = (tempL > 9) ? (tempL - 10 + 'A') : (tempL + '0');
	}
	*(exStr+imLen*2) = '\0';
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31216102032 将HEX数组数据转换成字符串静态指针返回
//*入口参数:	imLen--HEX字节数,最大为5,对应0xFFFFFFFF的BCD 42--94--96--72--95
//*             *imHex--HEX指针
//*返 回 值:	static*--11字节长度的静态数组指针
//*应用示例:	0x12 0xA2 0x32 0x0D -> "12A2320D\0x" length is 4
//*             0x12 0x34 0x56 -> "123456\0xxx"
//*说    明:	在转换后添加一个字串结束符'\0',故静态数组的长度设为11
//*==============================================|=====================|===================*
uint8_t* fg_ImHexReString(uint8_t imLen,uint8_t *imHex)
{
	uint8_t i = 0;
	uint8_t tempH,tempL;
	static int8_t reBuf[16];

	for(i=0;i<imLen;i++)
	{
		tempH = imHex[i] >> 4;
		tempL = imHex[i] & 0x0F;
	    reBuf[i*2] = (tempH > 9) ? (tempH - 10 + 'A') : (tempH + '0');
		reBuf[i*2+1] = (tempL > 9) ? (tempL - 10 + 'A') : (tempL + '0');
	}
	*(reBuf+imLen*2) = '\0';
	return reBuf;
}
//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31220230435 将U32型的数据转化为字符串
//*入口参数:	imPos--指定静态数组的输出位置
//*             imNum--需要转换的U32型数据
//*返 回 值:	static*--转换之后的字串指针
//*应用示例:	im:12345678(=0x075BCD15) -> re:"00075BCD15\0" 十六进制输出
//*             im:12345678(=0x075BCD15) -> re:"0012345678\0" 十进制输出
//*==============================================|=====================|===================*
uint8_t SpaceNumFlag = 1;							//F_:DI <1=>使用空格(默认) <0=>保留0,须设置
uint8_t DecStringFlag = 1;						   	//F_:DI <1=>十进制输出字符串(默认) <0=>十六进制输出字符串
uint8_t* fg_NumberReString(uint32_t imNum)
{
	uint8_t i=0;
	uint8_t reBuf[5];
	uint8_t *rePtr=NULL;
	if(DecStringFlag)
	{
		fg_LongToBcd(imNum,5,0,reBuf);				//F_:以十进制转为字串
		rePtr = fg_ImHexReString(5,reBuf);			//F_:指定16字节长度的静态数组的位置
	}
	else 
	{
		fg_CopyBytes_H2L(4,(uint8_t*)&imNum,reBuf);	//F_:以十六进制转为字串
		rePtr = fg_ImHexReString(4,reBuf);			//F_:指定16字节长度的静态数组的位置
	}
	if(SpaceNumFlag)
		for(i=0;rePtr[i]=='0';i++) rePtr[i] = ' ';
	SpaceNumFlag = 1;
	DecStringFlag = 1;		
	return rePtr;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31220232210 将U32型数据转换成浮点型的字符串,指定浮点数
//*入口参数:	imDigit--浮点位置,通常为2
//*             imLen--指定转换位数
//*             imNum--需要转换的U32型数据
//*返 回 值:	static*--转换之后的字串指针
//*==============================================|=====================|===================*
uint8_t* fg_NumReFloatStr(uint8_t imDigit,uint32_t imNum) 
{
	uint8_t *ptr = fg_NumberReString(imNum);			//im:0,12345678(=0x075BCD15)	re:"0012345678\0xxxxxxx"
	if(imDigit)
	{
		if(imNum==0) return "       0.00";
		fg_ShiftBytes_L2R(1,imDigit+1,ptr+10-imDigit);	//im:1,3,"0012345678\0xxxxxxx" -> "00123456778\0xxxxxx"
		ptr[10-imDigit] = '.';							//"00123456778\0xxxxxx" -> "00123456.78\0xxxxxx"
	}
	return ptr;

//	uint8_t i=0,k=0;
//	static uint8_t reBuf[12];
//	for(i=0;i<imDigit;i++)
//	{
//		reBuf[imLen-1-i] = imNum%10;
//		imNum /= 10;
//	}
//	if(imDigit) reBuf[imLen-1-imDigit] = '.';
//	k = imDigit?(imLen-2-imDigit):(imLen-1-imDigit);
//	reBuf[k] = imNum%10;
//	imNum /=10;
//	for(i=1;i<imLen-k;i++)
//	{
//		reBuf[k-i] = imNum%10;
//		imNum /=10;
//	}
//	for(i=0;i<imLen;i++) reBuf[i] += 48;
//	reBuf[4] = '.';
//	i=0;	
//	while(reBuf[i]==48&&i<3)
//	{
//		reBuf[i] = ' ';
//		i++;
//	} 
//	return reBuf;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31125152647 将字节流右移N个字节
//*入口参数:	imOffset--右移字节数
//*             imLen--字节流长度
//*双向参数:	*ieBytes--字节流指针
//*应用示例:	im={1,2,3,0,0} -> ex={1,2,1,2,3} offset=2,len=3
//*==============================================|=====================|===================*
void fg_ShiftBytes_L2R(uint8_t imOffset,uint8_t imLen,uint8_t *ieBytes)
{
	uint8_t i=0;
	for(i=imLen;i>0;i--)
	{
		*(ieBytes+i+imOffset-1) = *(ieBytes+i-1);	
	}
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31125155856 将字节流左移N个字节
//*入口参数:	imOffset--左移字节数
//*             imLen--字节流长度
//*双向参数:	*ieBytes--字节流指针
//*应用示例:	im={0,0,1,2,3} -> ex={1,2,3,2,3} offset=2 len=3
//*==============================================|=====================|===================*
void fg_ShiftBytes_R2L(uint8_t imOffset,uint8_t imLen,uint8_t *ieBytes)
{
	uint8_t i=0;
	for(i=imOffset;i<imLen+imOffset;i++)
	{
		*(ieBytes+i-imOffset) = *(ieBytes+i);
	}	
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31126164305 计算字节流的累加和
//*入口参数:	imLen--字节流长度
//*             *imBytes--字节流指针
//*返 回 值:	计算结果
//*==============================================|=====================|===================*
uint8_t fg_BytesSum(uint8_t imLen,uint8_t *imBytes)
{
	uint8_t i=0;
	uint8_t temp=*imBytes;
	for(i=1;i<imLen;i++) 
		temp += *(imBytes+i);
	return temp;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31126163943 计算字节流的异或和
//*入口参数:	imLen--字节流长度
//*             *imBytes--字节流指针
//*返 回 值:	计算结果
//*==============================================|=====================|===================*
uint8_t fg_BytesXor(uint8_t imLen,uint8_t *imBytes)
{
	uint8_t i=0;
	uint8_t temp=*imBytes;
	for(i=1;i<imLen;i++) 
		temp ^= *(imBytes+i);
	return temp;
}

//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID31219225027 将字节流全部取反
//*入口参数:	imLen--字节流长度
//*双向参数:	*ieBytes--字节流指针
//*==============================================|=====================|===================*
void fg_BytesCPL(uint8_t imLen,uint8_t *ieBytes)
{
   uint8_t i=0;
   for(i=0;i<imLen;i++)
      *(ieBytes+i) = ~(*(ieBytes+i));
}



//*==============================================|=====================|===================*
//*函数原型:    
//*编号功能:    CID30114101205 取绝对值
//*入口参数:	imNum1--操作数1
//*             imNum2--操作数2
//*返 回 值:	|imNum1-imNum2|
//*应用示例:	im:2,3 -> re:|2-3|=1
//*==============================================|=====================|===================*
uint16_t fg_MathAbsolute(int32_t imNum1,int32_t imNum2)
{
	return imNum1>imNum2?imNum1-imNum2:imNum2-imNum1;
}


/**********************************************************************|******************** 
*--------------------------------结束----------------------------------|-------------------*
***********************************************************************|*******************/ 



