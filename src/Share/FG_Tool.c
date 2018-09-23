
#include <string.h>
#include "FG_Tool.h"

//*********************************************************************|********************
//*-----------------------------���ú���-------------------------------|-------------------*
//*********************************************************************|********************

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID30109134950 ���ֽ����͵�ʱ��ת��ΪU32��ʱ�䷵��,���㴫��
//*��ڲ���:	*imBytes--�ֽ�����ʱ��,��ʽ:YYMMDDHHMMSS
//*�� �� ֵ:	U32--U32��ʱ��,��ʽ:
//*             ��B31..B21��B20..B17��B16..B12ʱB11..B6��B5..B0
//*˵    ��:	YY[0,2^11]
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
//*����ԭ��:    
//*��Ź���:    CID30109142907 ��U32��ʱ��ת��Ϊ�ֽ����з���,����ۿ�
//*��ڲ���:	imU32--U32��ʱ��
//*�� �� ֵ:	static*--�ֽ�����ʱ��,��ʽ:YYMMDDHHMMSS
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
//*����ԭ��:    
//*��Ź���:    CID30110145313 ������ת��Ϊ4Ԫ������
//*��ڲ���:	imU32--���������,�ΪU32��
//*�� �� ֵ:	static*--ת���������ָ��
//*Ӧ��ʾ��:	im:0x12345678 -> re:{0x12,0x34,0x56,0x78}
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
//�����ֽڽ��а��ֽڽ���
//��ڣ�*Byte1 *Byte2 �������������ֽ�
//		Byte1HL:1 ��һ���ֽڵĸ߰��ֽ� 0 ��һ���ֽڵĵͰ��ֽ�
//		Byte2HL:1 �ڶ����ֽڵĸ߰��ֽ� 0 �ڶ����ֽڵĵͰ��ֽ�
//���ڣ�*Byte1 *Byte2 ������������ֽ�
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
*��������:cp_RLBuf
*��������:ѭ������MoveTimes��
*��ڲ���:TemDataBuf�����ݣ�,BufLen(����������),MoveTimes(�ƶ�����)
*���ڲ���:TemDataBuf
*���ؽ��:��
********************************************************************************************/
void cp_RLBuf(unsigned char *TemDataBuf,unsigned char BufLen,unsigned char MoveTimes)
{
	unsigned char i,j;
	unsigned char uchar_TemV,uchar_LastBit;
	unsigned char *uchar_p;
	
	for (i=0;i<MoveTimes;i++)
	{
		uchar_p=TemDataBuf;
		uchar_LastBit=(*uchar_p & 0x80);                      //ȡ�õ�һ�ֽڵ����λ
		uchar_LastBit>>=7;                                     //�ŵ����λ��׼��װ�뵽���һ�ֽڵ����һλ
		for (j=0;j<BufLen;j++)
		{
			if (j==BufLen-1)                                   
			{                                                 //���һ�ֽ������LastBit
				*uchar_p<<=1;
				*uchar_p=*uchar_p | uchar_LastBit; 
			}
			else
			{ 
				uchar_TemV=(*(uchar_p+1) & 0x80);             //ȡ����һ�ֽڵ����λ
				uchar_TemV>>=7;                                //�ŵ���ߵ�λ��׼��װ�뵽��һ�ֽ�
				*uchar_p<<=1;                                  //����1λ
				*uchar_p=*uchar_p | uchar_TemV;               //������һ�ֽڵ����λ��Ϊ���λ
				uchar_p++;
			}
		} 
	}
}
/********************************************************************************************
*��������:cp_RRBuf
*��������:ѭ������MoveTimes��
*��ڲ���:TemDataBuf�����ݣ�,BufLen(����������),MoveTimes(�ƶ�����)
*���ڲ���:TemDataBuf
*���ؽ��:��
********************************************************************************************/
void cp_RRBuf(unsigned char *TemDataBuf,unsigned char BufLen,unsigned char MoveTimes)
{
	unsigned char i,j;
	unsigned char uchar_TemV,uchar_LastBit;
	unsigned char *uchar_p;
	
	for (i=0;i<MoveTimes;i++)
	{
		uchar_p=TemDataBuf;
		uchar_LastBit=(*(uchar_p+BufLen-1) & 0x01);    	//ȡ�����һ�ֽڵ����λ
		uchar_LastBit<<=7;                             	//�ŵ����λ��׼��װ�뵽��һ�ֽ�
		for(j=0;j<BufLen;j++)
		{
			uchar_TemV=(*uchar_p & 0x01);              	//ȡ�����һ�ֽڵ����λ
			uchar_TemV<<=7;                             //�ŵ����λ��׼��װ�뵽��һ�ֽ�
			*uchar_p>>=1;                               //����1λ
			*uchar_p=*uchar_p | uchar_LastBit;         	//������һ�ֽڵ����λ��Ϊ���λ
			uchar_p++;
			uchar_LastBit=uchar_TemV;
		} 
	}
}


//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31219221126 ����ֽ�����ָ�����ݵĸ���
//*��ڲ���:	imLen--�ֽ�������
//*             *imBytes--�ֽ���ָ��
//*             imTarget--ָ��������
//*�� �� ֵ:	result--ָ�����ֵĸ���,��imLen��ȱ�ʾ�ֽ���������ȫΪtarget
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
//*����ԭ��:    
//*��Ź���:    CID31112120722 ��ʼ���ֽ���
//*��ڲ���:	imLen--�ֽ�������
//*             *imPtr--�ֽ���ָ��
//*             imValue--��ʼ����ֵ
//*==============================================|=====================|===================*
void fg_InitBytes(uint8_t imLen,uint8_t *imPtr,uint8_t imValue)
{
	uint8_t i=0;
	for(i=0;i<imLen;i++) *(imPtr+i) = imValue;
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31112102623 ����N���ֽ�
//*��ڲ���:	imLen--N�Ĵ�С
//*             *imArray--�����Ƶ�Դ����
//*���ڲ���:	*exArray--����֮�������
//*Ӧ��ʾ��:	im:0x00 0x01 0x02 0x03 -> ex:0x00 0x01 0x02 0x03 Len is 4 
//*==============================================|=====================|===================*
void fg_CopyBytes(uint8_t imLen,uint8_t* imArray,uint8_t* exArray)
{
	uint8_t i = 0;
	for(i=0;i<imLen;i++)
		*(exArray+i) = *(imArray+i);	
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31112102908 ������N���ֽ�,imH->exL
//*��ڲ���:	imLen--N�Ĵ�С
//*             *imArray--�����Ƶ�Դ����
//*���ڲ���:	*exArray--����֮�������
//*Ӧ��ʾ��:	im:0x00 0x01 0x02 0x03 -> ex:0x03 0x02 0x01 0x00 Len is 4 
//*             im:0x00 0x01 0x02 0x03 -> ex:0x02 0x01 0x00 Len is 3
//*==============================================|=====================|===================*
void fg_CopyBytes_H2L(uint8_t imLen,uint8_t* imArray,uint8_t* exArray)
{
	uint8_t i = 0;
	for(i=0;i<imLen;i++)
		*(exArray+i) = *(imArray+imLen-i-1);	
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31112120343 �Ƚ��ֽ���(�ַ���,����,�ṹ��...)
//*��ڲ���:	imLen--�ֽ�������
//*             *str1--�ֽ���1
//*             *str2--�ֽ���2
//*�� �� ֵ:	<0=>���
//*             <!0=>����ȵ��ֽ�
//*Ӧ��ʾ��:	str1:0x01 0x02 0x03 0x04 <-> str2:0x04 0x03 0x02 0x01 Len is 4 return 1
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
//*����ԭ��:    
//*��Ź���:    CID31112115545 ����Ƚ��ֽ���(�ַ���,����,�ṹ��...)
//*��ڲ���:	imLen--�ֽ�������
//*             *str1--�ֽ���1
//*             *str2--�ֽ���2
//*�� �� ֵ:	<0=>���
//*             <!0=>����ȵ��ֽ�
//*Ӧ��ʾ��:	str1:0x01 0x02 0x03 0x04 <-> str2:0x04 0x03 0x02 0x01 Len is 4 return 0
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
//*����ԭ��:    
//*��Ź���:    CID3110801408 ��ȡ���������еĵ�һ��0(��������)
//*��ڲ���:	imNum--��Ҫ����������
//*�� �� ֵ:	<0..31=>��������ĵ�һ��0��λ��
//*             <0xFF=>�������ȫ1
//*��    ��:    20131126,����,�Ѳ���
//*Ӧ��ʾ��:	imNum=8493104 -> return 0
//*==============================================|=====================|===================*
uint8_t fg_GetFirst0ByBin(uint32_t imNum)
{
	uint8_t i = 0;
	for(i=0;i<32;i++)
		if(((imNum >> i)&1) == 0)				//F_:λ�߼��������ȼ���!!!
			return i;
	return 0xFF;
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31126112518 ��������ĵ�һ��1��λ��
//*��ڲ���:	imNum--Ŀ������
//*�� �� ֵ:	<0xFF=>ȫ0
//*             <n=>���
//*��    ��:    20131126,����,�Ѳ���
//*Ӧ��ʾ��:	imNum=8493104 -> return 4
//*==============================================|=====================|===================*
uint8_t fg_GetFirst1ByBin(uint32_t imNum)
{
	uint8_t i=0;
	for(i=0;i<32;i++)
		if(((imNum >> i)&1) == 1)				//F_:λ�߼��������ȼ���!!!
			return i;
	return 0xFF;
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31126160509 ��U32���ݵ�ĳһλ��1
//*��ڲ���:	im1--ָ��ĳλ
//*˫�����:	imNum--��Ҫ���õ�����
//*�� �� ֵ:	�������
//*��    ��:    20131126,����,�Ѳ���
//*Ӧ��ʾ��:	im(3,0) -> re(8)
//*˵    ��:	�˺������ı�ʵ�ε�imNum��ֵ,����ɷ���ֵ�ݴ�
//*��    ��:    20131206,����,�Ѳ���
//*==============================================|=====================|===================*
uint32_t fg_SetOneBit(uint8_t im1,uint32_t imNum)
{
	return imNum |= (1<<im1);
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31126160441 ��U32���ݵ�ĳһλ��0
//*��ڲ���:	im1--ָ��ĳλ
//*˫�����:	imNum--��Ҫ���������
//*�� �� ֵ:	�������
//*��    ��:    20131126,����,�Ѳ���
//*Ӧ��ʾ��:	im(3,255) -> re(247)  
//*˵    ��:	�˺������ı�ʵ�ε�imNum��ֵ,����ɷ���ֵ�ݴ�
//*��    ��:    20131206,����,�Ѳ���
//*==============================================|=====================|===================*
uint32_t fg_ClrOneBit(uint8_t im1,uint32_t imNum)
{
	return imNum &= ~(1<<im1);
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31206111425 ��ȡU32�������е�ָ��λ��ֵ
//*��ڲ���: 	imBitLen--��Ҫ��ȡ��λ����
//*             imOffset--��λ��ƫ��(����ڵ�0λ)
//*             imNum--U32��Ŀ������
//*�� �� ֵ:	��ȡ��ָ��λ��ֵ,Value=[0,imNum]
//*Ӧ��ʾ��:	im:5,1,0x12345678 -> re:11100b=0x1C
//*��    ��:    20131206,����,�Ѳ���
//*==============================================|=====================|===================*
uint32_t fg_GetBitsInU32(uint8_t imBitLen,uint8_t imOffset,uint32_t imNum)
{		  
	return ((imNum>>imOffset) & ((1<<imBitLen)-1));	
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31206143027 ����U32��������ָ��λ����ֵ
//*��ڲ���: 	imBitLen--��Ҫ��ȡ��λ����
//*             imOffset--��λ��ƫ��(����ڵ�0λ)
//*             imBitsValue--��Ҫ���õ�ָ��λ��ֵ 
//*˫�����:	*ieNum--U32Ŀ�������
//*�� �� ֵ:	���
//*Ӧ��ʾ��:	im:5,1,0,num(0x12345678) -> re:0x12345640 ex:num(0x12345640)
//*Ӧ��ʾ��:	im:1,16,1,num(0x0000000) -> re:0x00010000 ex:num(0x00010000)
//*             im:1,16,0,num(0xFFFFFFF) -> re:0xFFFEFFFF ex:num(0xFFFEFFFF)
//*˵    ��:	"<<"�����ȼ�����"+"
//*��    ��:    20131212,����,�Ѳ���
//*==============================================|=====================|===================*
uint32_t fg_SetBitsInU32(uint8_t imBitLen,uint8_t imOffset,uint32_t imBitsValue,uint32_t *ieNum)
{
	return *ieNum = (*ieNum&(-1<<imOffset+imBitLen))+
					(imBitsValue<<imOffset)+(*ieNum&((1<<imOffset)-1));
}						

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31127132536 ���ֽ����н�ȡ���4���ֽ�,ת����uint32_t�ͷ���
//*��ڲ���:	imTruLen--��ȡ����,���Ϊ4��Ӧuint32_t
//*             *imBytes--�ֽ���ָ��
//*�� �� ֵ:	result--���
//*˵    ��:	u32_--�����ķ���ֵΪuint32_t��
//*             LB--�͵�ֵ�洢��ֵ(���)
//*Ӧ��ʾ��:	0x01 0x02 0x03 0x04 -> 0x01020304	Len is 4
//*˵    ��:	ʹ��ǰ�ñ�֤imOffset���ܳ����ֽ����ĳ���
//*==============================================|=====================|===================*
uint32_t fg_BytesTruncByU32(uint8_t imTruLen,uint8_t *imBytes)
{
	uint8_t i = 0;
	uint32_t result = 0;
	for(i=0;i<imTruLen;i++)
	{
		result |= (*(imBytes+i) << (imTruLen-i-1)*8);
	}
//*С��ģʽ
//	for(i=0;i<imTruLen;i++)
//	{
//		result |= (*(imBytes+imOffset+i) << i*8);
//	}
	return result;	
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31127145311 ��һ��U32�����ݰ�BL(���ֽ���ǰ)ճ�����ֽ�����,����4��ʧ����(DB)
//*��ڲ���:	imPasLen--ճ���ĳ���,���4�ֽ�
//*             *imBytes--�ֽ���
//*             imU32Num--U32������
//*Ӧ��ʾ��:	im:0x12345678 -> ex:--34--56--78-- 	len:3
//*             im:0x12345678 -> ex:--56--78--		len:2		
//*˵    ��:	��ճ�������ݲ���4���ֽڵ�ʱ��,����ʧU32�е�"����"		
//*==============================================|=====================|===================*
void fg_U32PasteInBytes_DB(uint8_t imPasLen,uint32_t imU32Num,uint8_t *ieBytes)
{
	uint8_t i = 0;
	for(i=0;i<imPasLen;i++)
		*(ieBytes+imPasLen-1-i) = *((uint8_t*)&imU32Num+i);	
}					

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31204164510 ��һ��U32�����ݰ�BL(���ֽ���ǰ)ճ�����ֽ�����,����4��ʧС��(DS)
//*��ڲ���:	imPasLen--ճ���ĳ���,���4�ֽ�
//*             *imBytes--�ֽ���
//*             imU32Num--U32������
//*Ӧ��ʾ��:	im:0x12345678 -> ex:--12--34--56-- 	len:3
//*             im:0x12345678 -> ex:--12--34--		len:2
//*˵    ��:	��ճ�������ݲ���4���ֽڵ�ʱ��,����ʧU32�е�"С��"		
//*==============================================|=====================|===================*
void fg_U32PasteInBytes_DS(uint8_t imPasLen,uint32_t imU32Num,uint8_t *ieBytes)
{
	uint8_t i = 0;
	for(i=0;i<imPasLen;i++)
		*(ieBytes+i) = *((uint8_t*)&imU32Num+3-i);	
}

//*==============================================|=====================|===================*
//*����ԭ��:    uint8_t MakeXor(uint8_t *imData,uint8_t imB,uint8_t imE);
//*��Ź���:    CID30910160236 ������
//*��ڲ���:	*imData--��Ҫ��͵�����ָ��
//*             imB--��һ������
//*             imE--���һ������
//*�� �� ֵ:	result--���
//*Ӧ��ʾ��:    buf_comm[14] = MakeXor(buf_comm,2,13);
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
//*����ԭ��:    
//*��������:    ��������ֽ�������ϳ��ֻ���˫�����͵�����
//*��ڲ���:	imCLength--�ֽڸ���
//*		*imCData--�ֽ�����ָ��
//*�� �� ֵ:	lRev--����ֽ���ϳ�˫��������
//*˵    ��:	��Ϸ�ʽ�Ǹ��ֽ���ǰ
//*Ӧ��ʾ��:	0x01 0x02 0x04 0x08 -> 0x01020408	length is 4
//*��    ��:    20131102,����,�Ѳ���
//*==============================================|=====================|===================*
uint32_t fg_BytesToLong_L2B(uint8_t imCLength,uint8_t* imCData)		//F_:export ���� import���	
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
//*����ԭ��:        
//*��������:    ��һ�������ε����ݰ��ո��ֽ���ǰ��ֳɶ���ֽ�
//*��ڲ���:    imCLength--�ŷ��ֽڵĸ���
//*             imLData--��Ҫ��ֵĳ�������
//*�� �� ֵ:	*pCRev--���֮����ֽ�����ָ��,���ֽ���ǰ
//*Ӧ��ʾ��:    0x12345678 -> {0x12,0x34,0x56,0x78}    length is 4
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
//*����ԭ��:    
//*��Ź���:    CID31025164705 ��HEX��������ת�����ַ���
//*��ڲ���:	imLen--���鳤��
//*             *imHex--HEX����ָ��
//*���ڲ���:	*exStr--ת��֮���ָ��
//*Ӧ��ʾ��:	0x12 0xA2 0x32 0x0D -> "12A2320D" length is 4
//*˵    ��:	exStrӦ��������������,�䳤��Ӧ����imLen*2+1(����'\0')
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
//*����ԭ��:    
//*��Ź���:    CID31115212822 �����ֽ���
//*��ڲ���:	imBytesLen--�����ӵ��ֽ�������
//*             imAddLen--��Ҫ���ӵĳ���
//*             *imBytes--���ӵ��ֽ���ָ��
//*˫�����:	*ieBytes--�������ֽ���ָ��
//*��    ��:    20131115,����,�Ѳ���
//*Ӧ��ʾ��:	fg_BytesAddBytes(4,4,&buf,"fang") -> buf:"xxxxfangg"
//*==============================================|=====================|===================*
uint8_t* fg_BytesAddBytes(uint8_t imBytesLen,uint8_t imAddLen,uint8_t *ieBytes,uint8_t *imBytes)
{
	uint8_t i = 0;
	for(i=0;i<imAddLen;i++)
		*(ieBytes+imBytesLen+i) = *(imBytes+i);
	return ieBytes;
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31115211907 �����ֺ������λ����ASC��ʽ��ʾΪ�����벢��"Err:"
//*��ڲ���:	imNum--����
//*���ڲ���:	exPtr--�������ִ�ָ��
//*��    ��:    20131115,����,�Ѳ��� �����ֽ������ӷ�������!!
//*Ӧ��ʾ��:	fg_GetErrCodePtr(0x12345678,&buf) -> buf: "Err:5678"
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
//*����ԭ��:    
//*��Ź���:    CID31121152054 ���ֽ������Ƴ�һ��ָ�����ַ�,ֵ�ж�
//*��ڲ���:	imLen--�ֽ�������
//*           	imValue--��Ҫ�Ƴ���ֵ
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
//*����ԭ��:    
//*��Ź���:    CID31125111149 BCDתU32
//*��ڲ���:	imBcdLen--BCD����
//*             imLongLen--U32��Ч�ֽ���
//*             imSort--HEX�����ѭ�� <0=>L2B(���ֽ���ǰ) <1=>L2S(���ֽ���ǰ)
//*             *imBcd--��Ҫת����BCD����ָ��
//*�� �� ֵ:	long--BCDת��֮��Ľ��
//*Ӧ��ʾ��:	Bcd={0x12,0x34,0x56,0x78} -> U32=0xBC614E BcdLen=4,HexLen=3
//*             							 U32=0x00BC614E BcdLen=4,HexLen=4
//*��    ��:    20131204,����,�Ѳ���
//*==============================================|=====================|===================*
uint32_t fg_BcdToLong(uint8_t imBcdLen,uint8_t imLongLen,uint8_t imSort,uint8_t *imBcd)
{
	uint8_t hex[4]={0};
	fg_BcdToHex(imBcdLen,imLongLen,imSort,imBcd,hex);
	return fg_BytesToLong_L2B(imLongLen,hex);
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31122102036 BCDתHEX
//*��ڲ���:	imBcdLen--BCD����
//*             imHexLen--HEX�������
//*             imSort--HEX�����ѭ�� <0=>L2B(���ֽ���ǰ) <1=>L2S(���ֽ���ǰ)
//*             *imBcd--��Ҫת����BCD����ָ��
//*���ڲ���:	*exHex--ת��֮���HEX����ָ��
//*Ӧ��ʾ��:	Bcd:{0x12,0x34,0x56,0x78} -> Hex:{0xBC,0x61,0x4E} BcdLen=4,HexLen=3 
//*             							 Hex:{0x00,0xBC,0x61,0x4E} BcdLen=4,HexLen=4
//*==============================================|=====================|===================*
void fg_BcdToHex(uint8_t imBcdLen,uint8_t imHexLen,uint8_t imSort,uint8_t *imBcd,uint8_t *exHex)
{
	uint8_t  tempBuf[8]={0};    
	uint8_t  i,CurBCDPos;
	uint16_t TemInt,TemADDC;
	
	//�Ȱ����llong�ʹ����8�ֽ�����,��λ��ǰ
	for(CurBCDPos=0;CurBCDPos<imBcdLen;CurBCDPos++)
	{
		TemADDC=0;                              //TemADDC�����λ
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]*10;         		//*10
			TemInt+=TemADDC;                    //���Ͻ�λ
			tempBuf[i]=TemInt % 256;      		//���汾λ��ֵ
			TemADDC=TemInt / 256;               //�õ���λ
		}
		TemInt=*(imBcd+CurBCDPos);         		//���ϱ��ֽڵĸ�λ 
		TemInt&=0xf0;
		TemInt>>=4;
		TemADDC=TemInt;
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]+TemADDC;   		//���ӵ�HEX������
			tempBuf[i]=TemInt % 256;
			TemADDC=TemInt / 256;
		}
		TemADDC=0;                              //TemADDC�����λ
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]*10;         		//*10
			TemInt+=TemADDC;                    //���Ͻ�λ
			tempBuf[i]=TemInt % 256;      		//���汾λ��ֵ
			TemADDC=TemInt / 256;               //�õ���λ
		}
		TemInt=*(imBcd+CurBCDPos);         		//���ϱ��ֽڵĵ�λ 
		TemInt&=0x0f;
		TemADDC=TemInt;
		for(i=0;i<8;i++)
		{
			TemInt=tempBuf[i]+TemADDC;   		//���ӵ�HEX������
			tempBuf[i]=TemInt % 256;
			TemADDC=TemInt / 256;
		}
	}
	//���,HEX�ǵ�λ��ǰ
	if(imSort)	for(i=0;i<imHexLen;i++)	*(exHex+i)=tempBuf[i];
	//���,HEX��λ��ǰ
	else for(i=0;i<imHexLen;i++) *(exHex+i)=tempBuf[imHexLen-i-1];
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31122142910 ����BCD�����
//*��ڲ���:	imByte1--������
//*             imByte2--����
//*             imCarryFlag--�ϴ�����Ľ�λ��־,����һ�β��ò���
//*���ڲ���:	*exBytes--���
//*==============================================|=====================|===================*
uint8_t fg_BcdAdd(uint8_t imByte1,uint8_t imByte2,uint8_t imCarryFlag,uint8_t *exBytes)
{
	uint8_t uchar_TemL1,uchar_TemH1,uchar_TemL2,uchar_TemH2,TemChar;
	uint8_t uchar_ADDCF=0;
	
	//�ȷֽ�ɸ��ߵ�λ
	uchar_TemL1=imByte1 & 0x0f;
	uchar_TemH1=imByte1 & 0xf0;
	uchar_TemH1>>=4;
	uchar_TemL2=imByte2 & 0x0f;   
	uchar_TemH2=imByte2 & 0xf0;
	uchar_TemH2>>=4;
	
	//��λ���
	TemChar=uchar_TemL1+uchar_TemL2+imCarryFlag;
	//�ж��Ƿ���AC��λ��>9,��������4λ��6���൱��DA������
	if(TemChar>9) TemChar=TemChar+6;
	uchar_TemL1=TemChar & 0x0f;
	//��λ���
	TemChar>>=4;
	TemChar=uchar_TemH1+uchar_TemH2+TemChar;
	//�ж��Ƿ���C��λ��>9,��������4λ��6���൱��DA������
	if(TemChar>9) TemChar=TemChar+6;
	uchar_TemH1=TemChar;
	uchar_TemH1<<=4;
	//ȡ����һ��λ��Ϊ�������
	TemChar>>=4;
	uchar_ADDCF = TemChar?1:0;
	//�õ�BCD���
	*exBytes=uchar_TemH1 | uchar_TemL1;
	
	return uchar_ADDCF;
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31122133818 U32��������ת��ΪBCD
//*��ڲ���:	imU32Num--��Ҫת����U32������
//*             imBcdOutLen--ָ�����BCD�ĳ���,���5�ֽ�(U32�͵������ת��Ϊ5�ֽڵ�BCD)
//*             imSort--ָ��BCD������ֽ��� <0=>_BS(�Ӵ�С) <1=>_SB(��С����)
//*���ڲ���:	*exBcdBuf--�����BCDָ��
//*==============================================|=====================|===================*
void fg_LongToBcd(uint64_t imU32Num,uint8_t imBcdOutLen,uint8_t imSort,uint8_t *exBcdBuf)
{
	uint64_t tempOut=0;
	uint8_t *ptrTemp=NULL;   
	uint8_t i,j,tempLen;
	uint8_t *sourBuf=(uint8_t *)&tempOut;   //Ҫת����LLONG�����ݵ�ָ��
	uint8_t outBitTemp=0;
	uint8_t tempBuf[8]={0};

	//��ʼ��
	tempOut=imU32Num;
	for(i=0;i<64;i++)
	{    
		//ѭ�����ƣ�ȡ��λ��
		outBitTemp = *(sourBuf+7)&0x80 ? 1:0;
		tempOut<<=1;
		for(j=0;j<8;j++) 
		outBitTemp=fg_BcdAdd(tempBuf[j],tempBuf[j],outBitTemp,&tempBuf[j]);
	}
	
	//��󳤶�ֻ����8
	tempLen=imBcdOutLen;
	if(tempLen>8) tempLen=8;
	ptrTemp=exBcdBuf;
	//׼�����
	for(i=0;i<tempLen;i++,ptrTemp++)
	{
		if(!imSort)	*ptrTemp=tempBuf[tempLen-i-1];	//F_:�Ӵ�С
		else		*ptrTemp=tempBuf[i];			//F_:��С����
	}
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31122140452 ��HEX����ת����BCD����
//*��ڲ���:	imHexLen--HEX���ݳ���
//*             imBcdLen--ת��֮��BCD���ݳ���(����imHexLen��1)
//*             imSort--BCD����ֽ��� <0=>_BS(�Ӵ�С) <1=>_SB(��С����)
//*             *imHex--HEX����ָ��
//*���ڲ���:	*exBcd--ת��֮��BCD����ָ��	
//*==============================================|=====================|===================*
void fg_HexToBcd(uint8_t imHexLen,uint8_t imBcdLen,uint8_t imSort,uint8_t *imHex,uint8_t *exBcd)
{
	uint64_t u64Temp=0;
	fg_CopyBytes_H2L(imHexLen,imHex,(uint8_t*)&u64Temp);
	fg_LongToBcd(u64Temp,imBcdLen,imSort,exBcd);
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID30101150427 ��һ���ֽڵ�HEX��ת����DEC
//*��ڲ���:	imHex--HEX��
//*�� �� ֵ:	reDec--DEC��
//*Ӧ��ʾ��:	im:0x14 -> re:14
//*==============================================|=====================|===================*
uint8_t fg_HexReDec(uint8_t imHex)
{
	return (imHex/16)*10+imHex%16;
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31220161801 ��HEX����ת����U32����ֵ,���4�ֽ�
//*��ڲ���:	imLen--ָ��HEX���鳤��
//*				*imHex--HEX����ָ��
//*�� �� ֵ:	number--ת��֮�����ֵ
//*Ӧ��ʾ��:
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
//*����ԭ��:    
//*��Ź���:    CID31221173306 ����������ַ���,ת��������,ָ������С����λ��
//*��ڲ���:	*imStr--�ִ�ָ��
//*             imDig--ָ����Ч�����С����λ��,ͨ����2λ,2λ֮��Ľ����账��
//*�� �� ֵ:	result--����֮��Ľ��
//*˵    ��:	imStrӦ��Ҫ��֤,���ֻ�ܺ���һ��С����
//*Ӧ��ʾ��:	im:"1234.5678",2 -> re:123456
//*             im:"123456",2 -> re:12345600
//*==============================================|=====================|===================*
uint32_t fg_InputStrToNum(uint8_t *imStr,uint8_t imDigit)
{
	uint8_t i=0;
	uint8_t dotPos=0;	   					//F_:С�����λ��,�������strlen(imStr)��ʾ��С����
	uint8_t floatCnt=0;						//F_:С����λ��
	uint32_t result=0;						//F_:�������ֵĽ��
	uint32_t floatValue=0;					//F_:С�����ֵĽ��

	for(i=0;imStr[i]&&imStr[i]!='.';i++) 
		dotPos = i+1;						//F_:С�������ִ��е��±�
	for(i=0;i<dotPos&&imStr[i];i++)
	{
		result *= 10;
		result += imStr[i]-'0';
	}
	for(i=0;i<imDigit;i++,result*=10);		//F_:����10^imDigit��
	floatCnt = strlen(imStr)-dotPos-1;		//F_:С����λ��
	for(i=0;floatCnt&&i<imDigit;i++)		//F_:����С�����ֵ�ֵ
	{										
		if(imStr[dotPos+1+i]) 
			floatValue += imStr[dotPos+1+i]-'0';
		if(i==imDigit-1) break;
		floatValue *= 10;
	}

	return result+floatValue;
}
//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31025164705 ��HEX��������ת�����ַ���
//*��ڲ���:	imLen--���鳤��
//*             *imHex--HEX����ָ��
//*���ڲ���:	*exStr--ת��֮���ָ��
//*Ӧ��ʾ��:	0x12 0xA2 0x32 0x0D -> "12A2320D" length is 4
//*˵    ��:	exStrӦ��������������,�䳤��Ӧ����imLen*2+1(����'\0')
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
//*����ԭ��:    
//*��Ź���:    CID31216102032 ��HEX��������ת�����ַ�����ָ̬�뷵��
//*��ڲ���:	imLen--HEX�ֽ���,���Ϊ5,��Ӧ0xFFFFFFFF��BCD 42--94--96--72--95
//*             *imHex--HEXָ��
//*�� �� ֵ:	static*--11�ֽڳ��ȵľ�̬����ָ��
//*Ӧ��ʾ��:	0x12 0xA2 0x32 0x0D -> "12A2320D\0x" length is 4
//*             0x12 0x34 0x56 -> "123456\0xxx"
//*˵    ��:	��ת�������һ���ִ�������'\0',�ʾ�̬����ĳ�����Ϊ11
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
//*����ԭ��:    
//*��Ź���:    CID31220230435 ��U32�͵�����ת��Ϊ�ַ���
//*��ڲ���:	imPos--ָ����̬��������λ��
//*             imNum--��Ҫת����U32������
//*�� �� ֵ:	static*--ת��֮����ִ�ָ��
//*Ӧ��ʾ��:	im:12345678(=0x075BCD15) -> re:"00075BCD15\0" ʮ���������
//*             im:12345678(=0x075BCD15) -> re:"0012345678\0" ʮ�������
//*==============================================|=====================|===================*
uint8_t SpaceNumFlag = 1;							//F_:DI <1=>ʹ�ÿո�(Ĭ��) <0=>����0,������
uint8_t DecStringFlag = 1;						   	//F_:DI <1=>ʮ��������ַ���(Ĭ��) <0=>ʮ����������ַ���
uint8_t* fg_NumberReString(uint32_t imNum)
{
	uint8_t i=0;
	uint8_t reBuf[5];
	uint8_t *rePtr=NULL;
	if(DecStringFlag)
	{
		fg_LongToBcd(imNum,5,0,reBuf);				//F_:��ʮ����תΪ�ִ�
		rePtr = fg_ImHexReString(5,reBuf);			//F_:ָ��16�ֽڳ��ȵľ�̬�����λ��
	}
	else 
	{
		fg_CopyBytes_H2L(4,(uint8_t*)&imNum,reBuf);	//F_:��ʮ������תΪ�ִ�
		rePtr = fg_ImHexReString(4,reBuf);			//F_:ָ��16�ֽڳ��ȵľ�̬�����λ��
	}
	if(SpaceNumFlag)
		for(i=0;rePtr[i]=='0';i++) rePtr[i] = ' ';
	SpaceNumFlag = 1;
	DecStringFlag = 1;		
	return rePtr;
}

//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID31220232210 ��U32������ת���ɸ����͵��ַ���,ָ��������
//*��ڲ���:	imDigit--����λ��,ͨ��Ϊ2
//*             imLen--ָ��ת��λ��
//*             imNum--��Ҫת����U32������
//*�� �� ֵ:	static*--ת��֮����ִ�ָ��
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
//*����ԭ��:    
//*��Ź���:    CID31125152647 ���ֽ�������N���ֽ�
//*��ڲ���:	imOffset--�����ֽ���
//*             imLen--�ֽ�������
//*˫�����:	*ieBytes--�ֽ���ָ��
//*Ӧ��ʾ��:	im={1,2,3,0,0} -> ex={1,2,1,2,3} offset=2,len=3
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
//*����ԭ��:    
//*��Ź���:    CID31125155856 ���ֽ�������N���ֽ�
//*��ڲ���:	imOffset--�����ֽ���
//*             imLen--�ֽ�������
//*˫�����:	*ieBytes--�ֽ���ָ��
//*Ӧ��ʾ��:	im={0,0,1,2,3} -> ex={1,2,3,2,3} offset=2 len=3
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
//*����ԭ��:    
//*��Ź���:    CID31126164305 �����ֽ������ۼӺ�
//*��ڲ���:	imLen--�ֽ�������
//*             *imBytes--�ֽ���ָ��
//*�� �� ֵ:	������
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
//*����ԭ��:    
//*��Ź���:    CID31126163943 �����ֽ���������
//*��ڲ���:	imLen--�ֽ�������
//*             *imBytes--�ֽ���ָ��
//*�� �� ֵ:	������
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
//*����ԭ��:    
//*��Ź���:    CID31219225027 ���ֽ���ȫ��ȡ��
//*��ڲ���:	imLen--�ֽ�������
//*˫�����:	*ieBytes--�ֽ���ָ��
//*==============================================|=====================|===================*
void fg_BytesCPL(uint8_t imLen,uint8_t *ieBytes)
{
   uint8_t i=0;
   for(i=0;i<imLen;i++)
      *(ieBytes+i) = ~(*(ieBytes+i));
}



//*==============================================|=====================|===================*
//*����ԭ��:    
//*��Ź���:    CID30114101205 ȡ����ֵ
//*��ڲ���:	imNum1--������1
//*             imNum2--������2
//*�� �� ֵ:	|imNum1-imNum2|
//*Ӧ��ʾ��:	im:2,3 -> re:|2-3|=1
//*==============================================|=====================|===================*
uint16_t fg_MathAbsolute(int32_t imNum1,int32_t imNum2)
{
	return imNum1>imNum2?imNum1-imNum2:imNum2-imNum1;
}


/**********************************************************************|******************** 
*--------------------------------����----------------------------------|-------------------*
***********************************************************************|*******************/ 



