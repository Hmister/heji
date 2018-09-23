#ifndef __UART1_H_
#define __UART1_H_

extern unsigned char gUart1_RecvFlag;
extern unsigned char gUart1_RecvBuf[40];

void Send1Data(unsigned int dat);
void Send1String(char *s);
void Uart1Init(void);
void Send1Datas(unsigned char *s,unsigned int len);

int Recv1Bytes(unsigned char *Data,int len,unsigned int time);

#endif