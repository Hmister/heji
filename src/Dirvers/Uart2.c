/**====================================================================*
*						STC15系列单片机串口2驱动
*----------------------------------------------------------------------*
*文 件 名:    Uart2.c
*文件描述:    串口2收发以及端口的映射
*软件版本:    V1.0
*硬件环境:    STC15W4K32S4
*创 建 人:    方规  526293582@qq.com(邮箱) 13477037750(手机)
*最后修改:    2017/06/23 11:55
*功能说明:    中断的方式实现发送和接收,实现端口可映射
-----------------------------------------------------------------------*
*外部资源:	 
1.	1ms定时计数器,用于接收是超时判断
2.	Uart2_PORT宏或变量,用来控制串口0使用哪个端口
*修改说明:
V1.0 ---------> V1.1
1.	P3.7 P1.7作为串口0发送脚后必须要设置成强推挽输出,P1.6作为串口输入脚必须要
	设置开漏,才能正常使用
-----------------------------------------------------------------------*
*硬件连接:
			WZ01_SN_A_V1.0			USB转TTL模块			PC串口助手
				GND		-------------	GND			
				P3.0	-------------	TX	
				P3.1	-------------	RX		-------		USB		(Uart2_PORT=1)
				P1.6	-------------	TX	
				P1.7	-------------	RX		-------		USB		(Uart2_PORT=2)
				P3.6	-------------	TX	
				P3.7	-------------	RX		-------		USB		(Uart2_PORT=其他)
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"
#include "Uart2.h"


#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
#define S2RB8 0x04              //S2CON.2
#define S2TB8 0x08              //S2CON.3
#define S2_S0 0x01              //P_SW2.0


static bit busy=0;

uchar gUart2_RecvFlag = 0;
uchar gUart2_RecvBuf[40] = 0;

void Send2Data(BYTE dat);
void Send2String(char *s);


void Uart2Init()
{
#if (Uart2_PORT==1)
    P_SW2 &= ~S2_S0;            //S2_S0=0 (P0.0/RxD3, P0.1/TxD3)
#elif (Uart2_PORT==2)	
	P_SW2 |= S2_S0;             //S2_S0=1 (P5.0/RxD3_2, P5.1/TxD3_2)
#endif
//使用定时器2作为波特率发生器		
    S2CON = 0x50;               //接收使能,B5设置为1
	
    T2L = (65536 - (FOSC/4/BAUD));   	//设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR |= 0x10;                		//使能定时器2
	AUXR |= 0x04;						//T2为1T模式

	
    IE2 |= 0x01;                 		//使能串口2中断
    EA = 1;
	
	Send2String("Uart2 Init!\r\n");
}

/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart2() interrupt 8 using 1
{
	static uchar i = 1;
	uchar temp = 0;
//	Send1Data('5');
	if(S2CON & S2RI)
	{
		S2CON &= ~S2RI;
		if(S2BUF == '\n')			//当发送了"\r\n"时,去掉"\n",部分串口助手软件会将换行替换成"\r\n"
		{
			temp = S2BUF;
			return;
		}
		gUart2_RecvBuf[i] = S2BUF;
		if(gUart2_RecvBuf[i] == '\r')
		{
			gUart2_RecvBuf[0] = i;	//第0个字节保存字串的长度
			gUart2_RecvFlag = 1;
			i = 1;
		}
		else
		{
			i++;
		}		
	}
    if(S2CON & S2TI)
    {
		S2CON &= ~S2TI;
		busy = 0;
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void Send2Data(BYTE dat)
{
//	Send1Data('3');
	while(busy);
	busy = 1;
	S2BUF = dat;         
//	Send1Data('4');
}

void Send2Datas(uchar *s,uint len)
{
	
	BUZZER = !BUZZER;
	if(len==0){
		Send2String(s);
		return;
	}
	while(len){
		Send2Data(*s++);
		if(*s == '\r' || *s == '\0')
			break;
		len--;
	}
}

/*----------------------------
发送字符串
----------------------------*/
void Send2String(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        Send2Data(*s++);         //发送当前字符
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
int Recv2Bytes(unsigned char *Data,int len,unsigned int time)
{
	unsigned int i=0;
	unsigned long curtime = System1MsCnt;
	while(1){
		if(S2CON & S2RI){			
			Data[i++] = S2BUF;
			S2CON &= ~S2RI;
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