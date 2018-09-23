#ifndef __UART3_H_
#define __UART3_H_


extern uchar gUart3_RecvFlag;
extern uchar gUart3_RecvBuf[40];

void Send3Data(unsigned int dat);
void Send3String(char *s);
void Uart3Init(void);
void Send3Datas(unsigned char *s,unsigned int len);

int Recv3Bytes(unsigned char *Data,int len,unsigned int time);

#endif