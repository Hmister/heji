#ifndef __UART4_H_
#define __UART4_H_


extern uchar gRecv4Flag;
extern uchar gRecv4Buf[40];

void Send4Data(unsigned int dat);
void Send4String(char *s);
void Uart4Init(void);
void Send4Datas(unsigned char *s,unsigned int len);

int Recv4Bytes(unsigned char *Data,int len,unsigned int time);

#endif