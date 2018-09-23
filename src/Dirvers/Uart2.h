#ifndef __UART2_H_
#define __UART2_H_


extern uchar gRecv2Flag;
extern uchar gRecv2Buf[40];

void Send2Data(unsigned int dat);
void Send2String(char *s);
void UART2Init(void);
void Send2Datas(unsigned char *s,unsigned int len);

int Recv2Bytes(unsigned char *Data,int len,unsigned int time);

#endif