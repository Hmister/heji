/**====================================================================*
*						STC15系列单片机串口4驱动
*----------------------------------------------------------------------*
*文 件 名:    Uart.c
*文件描述:    串口4收发以及端口的映射
*软件版本:    V1.0
*硬件环境:    STC15W4K32S4
*创 建 人:    方规  526293582@qq.com(邮箱) 13477037750(手机)
*最后修改:    2017/06/23 11:55
*功能说明:    中断的方式实现发送和接收,实现端口可映射
-----------------------------------------------------------------------*
*外部资源:	 
1.	1ms定时计数器,用于接收是超时判断
2.	Uart4_PORT宏或变量,用来控制串口0使用哪个端口
*修改说明:
V1.0 ---------> V1.1
1.	P3.7 P1.7作为串口0发送脚后必须要设置成强推挽输出,P1.6作为串口输入脚必须要
	设置开漏,才能正常使用
-----------------------------------------------------------------------*
*硬件连接:
			WZ01_SN_A_V1.0			USB转TTL模块			PC串口助手
				GND		-------------	GND			
				P3.0	-------------	TX	
				P3.1	-------------	RX		-------		USB		(Uart4_PORT=1)
				P1.6	-------------	TX	
				P1.7	-------------	RX		-------		USB		(Uart4_PORT=2)
				P3.6	-------------	TX	
				P3.7	-------------	RX		-------		USB		(Uart4_PORT=其他)
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"


#define S4_S0 0x02              //P_SW2.1
#define S4RI  0x01              //S4CON.0
#define S4TI  0x02              //S4CON.1
#define S4RB8 0x04              //S4CON.2
#define S4TB8 0x08              //S4CON.3


static bit busy;

uchar gRecv4Flag = 0;
uchar gRecv4Buf[40] = 0;

void Send4Data(BYTE dat);
void Send4String(char *s);


void Uart4Init()
{
#if (UART4_PORT==1)
    P_SW2 &= ~S4_S0;            //S4_S0=0 (P0.2/RxD4, P0.3/TxD4)
#elif (UART4_PORT==2)	
	P_SW2 |= S4_S0;             //S4_S0=1 (P5.2/RxD4_2, P5.3/TxD4_2)
#endif
//使用定时器2作为波特率发生器		
//	S4CON = 0x10;               //接收使能,B5设置为1
//    T2L = (65536 - (FOSC/4/BAUD));   	//设置波特率重装值
//    T2H = (65536 - (FOSC/4/BAUD))>>8;
//    AUXR |= 0x10;              	//使能定时器2
//	  AUXR |= 0x04;					//T2为1T模式
	
//使用定时器4作为波特率发生器
	S4CON = 0x50;               	//8位可变波特率
    T4L = (65536 - (FOSC/4/BAUD));  //设置波特率重装值
    T4H = (65536 - (FOSC/4/BAUD))>>8;
    T4T3M |= 0x20;              	//定时器4为1T模式
    T4T3M |= 0x80;              	//定时器4开始计时

//用于NFC时,不能使用中断	
//    IE2 |= 0x10;               		//使能串口4中断
//    EA = 1;

    Send1String("Uart4 Init!\r\n");
}

/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart4() interrupt 18 using 1
{
	static uchar i = 1;
	uchar temp = 0;
	if(S4CON & S4RI)
	{
		S4CON &= ~S4RI;
		if(S4BUF == '\n')			//当发送了"\r\n"时,去掉"\n",部分串口助手软件会将换行替换成"\r\n"
		{
			temp = S4BUF;
			return;
		}
		gRecv4Buf[i] = S4BUF;
		if(gRecv4Buf[i] == '\r')
		{
			gRecv4Buf[0] = i;	//第0个字节保存字串的长度
			gRecv4Flag = 1;
			i = 1;
		}
		else
		{
			i++;
		}		
	}
//    if (S4CON & S4TI)
//    {
//		S4CON &= ~S4TI;
//    }
}

/*----------------------------
发送串口数据
----------------------------*/
void Send4Data(BYTE dat)
{
    S4BUF = dat;                //写数据到UART数据寄存器
	while(!(S4CON & S4TI));		//等待发送完毕,然后再将发送中断标志清零
	
	S4CON &= ~S4TI;	
}

void Send4Datas(uchar *s,uint len)
{
	
//	BUZZER = !BUZZER;
	if(len==0){
		Send4String(s);
		return;
	}
	while(len){
		Send4Data(*s++);
//		if(*s == '\r' || *s == '\0')
//			break;
		len--;
	}
}

/*----------------------------
发送字符串
----------------------------*/
void Send4String(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        Send4Data(*s++);         //发送当前字符
    }
}

//*-------------------------------------------------------------
//*功能说明:	串口接收函数,使用T3超时判定
//*入口参数:	*Data--接收缓存,要足够大
//*				len--需要接收的长度,0表示接收最大长度
//*				time--超时长度,单位ms
//*返 回 值:	收到的字节数
//*注    意:使用此函数时,必须要将中断功能关闭,否则标志为在中断中清除了
//*-------------------------------------------------------------
extern unsigned long System1MsCnt;
int Recv4Bytes(unsigned char *Data,int len,unsigned int time)
{
	unsigned int i=0;
	unsigned long curtime = System1MsCnt;
	while(1){
		if(S4CON & S4RI){			
			Data[i++] = S4BUF;
			S4CON &= ~S4RI;
		}
		if(len!=0 && i==len+1){			//接收到了指定的长度
			return len;
		}
		if(System1MsCnt-curtime>time){
			break;						//超时,接收多少字节就返回多少字节
		}
	}
	return i;
}