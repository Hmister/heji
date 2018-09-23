

#ifndef FG_TOOL_H
#define FG_TOOL_H

#define uint64_t	unsigned long
#define uint32_t	unsigned long
#define uint16_t	unsigned int
#define uint8_t		unsigned char
	
#define int32_t		long
#define int16_t		int
#define int8_t		char

void zk_2BytesXhg(unsigned char *Byte1,unsigned char *Byte2,unsigned char Byte1HL,unsigned char Byte2HL);
void cp_RLBuf(unsigned char *TemDataBuf,unsigned char BufLen,unsigned char MoveTimes);
void cp_RRBuf(unsigned char *TemDataBuf,unsigned char BufLen,unsigned char MoveTimes);


uint32_t fg_CheckTimer(uint32_t imTimer);

void fg_InitBytes(uint8_t imLen,uint8_t *imPtr,uint8_t imValue);
void fg_CopyBytes(uint8_t imLen,uint8_t* imArray,uint8_t* exArray);
void fg_CopyBytes_H2L(uint8_t imLen,uint8_t* imArray,uint8_t* exArray);
void fg_LongToBytes_B2L(uint8_t imCLength,uint32_t imLData,uint8_t* exPData);
void fg_BcdToHex(uint8_t imBcdLen,uint8_t imHexLen,uint8_t imSort,uint8_t *imBcd,uint8_t *exHex);
void fg_LongToBcd(uint64_t imU32Num,uint8_t imBcdOutLen,uint8_t imSort,uint8_t *exBcdBuf);
void fg_HexToBcd(uint8_t imHexLen,uint8_t imBcdLen,uint8_t imSort,uint8_t *imHex,uint8_t *exBcd);
void fg_HexToString(uint8_t imLen,uint8_t *imHex,uint8_t *exStr);
void fg_ShiftBytes_L2R(uint8_t imOffset,uint8_t imLen,uint8_t *ieBytes);
void fg_ShiftBytes_R2L(uint8_t imOffset,uint8_t imLen,uint8_t *ieBytes);
void fg_U32PasteInBytes_DB(uint8_t imPasLen,uint32_t imU32Num,uint8_t *ieBytes);
void fg_U32PasteInBytes_DS(uint8_t imPasLen,uint32_t imU32Num,uint8_t *ieBytes);
void fg_BytesCPL(uint8_t imLen,uint8_t *imBytes);
void fg_CalcuMac(uint32_t imPosttc,uint8_t* imTime,uint8_t *exCodePin);

uint8_t fg_BytesCompare(uint8_t imLen,uint8_t *str1,uint8_t *str2);
uint8_t fg_BytesCompare_L2H(uint8_t imLen,uint8_t *str1,uint8_t *str2);
uint8_t fg_MakeXor(uint8_t *imData,uint8_t imB,uint8_t imE);
uint8_t fg_GetFirst1ByBin(uint32_t imNum);
uint8_t fg_GetFirst0ByBin(uint32_t imNum);
uint8_t fg_BytesSum(uint8_t imLen,uint8_t *imBytes);
uint8_t fg_BytesXor(uint8_t imLen,uint8_t *imBytes);
uint8_t fg_CheckBytes(uint8_t imLen,uint8_t *imBytes,uint8_t imTarget);
uint8_t fg_HexReDec(uint8_t imHex);


uint32_t fg_BytesToLong_L2B(uint8_t imCLength,uint8_t* imCData);
uint32_t fg_BytesTruncByU32(uint8_t imTruLen,uint8_t *imBytes);
uint32_t fg_BcdToLong(uint8_t imBcdLen,uint8_t imLongLen,uint8_t imSort,uint8_t *imBcd);
uint32_t fg_SetOneBit(uint8_t im1,uint32_t imNum);
uint32_t fg_ClrOneBit(uint8_t im1,uint32_t imNum);
uint32_t fg_GetBitsInU32(uint8_t imBitLen,uint8_t imOffset,uint32_t imNum);
uint32_t fg_SetBitsInU32(uint8_t imBitLen,uint8_t imOffset,uint32_t imBitsValue,uint32_t *ieNum);
uint32_t fg_HexReNumber(uint8_t imLen,uint8_t *imHex);
uint32_t fg_InputStrToNum(uint8_t *imStr,uint8_t imDigit);
uint32_t fg_TimeBytesReU32(uint8_t *imBytes);
uint16_t fg_MathAbsolute(int32_t imNum1,int32_t imNum2);

uint8_t* fg_GetErrCodePtr(uint32_t imNum,uint8_t *exPtr);
uint8_t* fg_ImHexReString(uint8_t imLen,uint8_t *imHex);
uint8_t* fg_NumberReString(uint32_t imNum);
uint8_t* fg_NumReFloatStr(uint8_t imDigit,uint32_t imNum); 
uint8_t* fg_TimeU32ReBytes(uint32_t imU32);
uint8_t* fg_U32ReBytes(uint8_t imU32);
#endif


