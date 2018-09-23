/**====================================================================*
*						STC15系列单片机串口0驱动
*----------------------------------------------------------------------*
*文 件 名:    Uart.c
*文件描述:    串口0收发以及端口的映射
*软件版本:    V1.1
*硬件环境:    STC15W4K32S4
*创 建 人:    方规  526293582@qq.com(邮箱) 13477037750(手机)
*最后修改:    2017/06/13 11:08
*功能说明:    查询的方式实现发送和接收,实现端口可映射
-----------------------------------------------------------------------*
*外部资源:	 
1.	1ms定时计数器,用于接收是超时判断
2.	UART0_PORT宏或变量,用来控制串口0使用哪个端口
*修改说明:
V1.0 ---------> V1.1
1.	P3.7 P1.7作为串口0发送脚后必须要设置成强推挽输出,P1.6作为串口输入脚必须要
	设置开漏,才能正常使用
-----------------------------------------------------------------------*
*硬件连接:
			WZ01_ZL_A_V1.0			USB转TTL模块			PC串口助手
				GND		-------------	GND			
				P3.0	-------------	TX	
				P3.1	-------------	RX		-------		USB		(UART0_PORT=1)
				P1.6	-------------	TX	
				P1.7	-------------	RX		-------		USB		(UART0_PORT=2)
				P3.6	-------------	TX	
				P3.7	-------------	RX		-------		USB		(UART0_PORT=其他)
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"


#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

static bit busy;

uchar gUart1_RecvFlag = 0;			
uchar gUart1_RecvBuf[40] = 0;

void Send1Data(BYTE dat);
void Send1String(char *s);


void Uart1Init()
{
#if (UART0_PORT==1)
    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
#elif (UART0_PORT==2)
	ACC = P_SW1;
	ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
	ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
	P_SW1 = ACC;  

//设置P3.7为强推挽模式,否则作为串口发送脚,不能正常工作
	P3M1 &= ~(1<<7);
	P3M0 |= (1<<7); 

#elif (UART0_PORT==3)
	ACC = P_SW1;
	ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
	ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
	P_SW1 = ACC;  
	
//设置P16为准双向口,P17为推挽输出,作为串口0使用时,一定要设置
	P1M1 &= ~(1<<6);
	P1M0 &= ~(1<<6);
	
	P1M1 &= ~(1<<7);
	P1M0 |= (1<<7);
#endif
	
    SCON |= 0x50;                //8位可变波特率
    AUXR |= 0x40;                //定时器1为1T模式, 使用|=导致串口不能使用
	AUXR &= 0xFE;				//波特率发生器选择: bit0<1=>定时器2 <0=>定时器1
	
    TMOD = 0x00;                //定时器1为模式0(16位自动重载)
    TL1 = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    TH1 = (65536 - (FOSC/4/BAUD))>>8;
    TR1 = 1;                    //定时器1开始启动
    ES = 1;                   //使能串口中断  关闭中断,否则Recv0Bytes()接收异常
    EA = 1;

    Send1String("STC15F2K60S2\r\nUart0 Init!\r\n");
}



/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart1() interrupt 4 using 1
{
	static uchar i = 1;
	uchar temp = 0;
	if(RI)
	{
		RI = 0;
		if(SBUF == '\n')			//当发送了"\r\n"时,去掉"\n",部分串口助手软件会将换行替换成"\r\n"
		{
			temp = SBUF;
			return;
		}
		gUart1_RecvBuf[i] = SBUF;
		if(gUart1_RecvBuf[i] == '\r')
		{
			gUart1_RecvBuf[0] = i;	//第0个字节保存字串的长度
			gUart1_RecvFlag = 1;
			i = 1;
		}
		else
		{
			i++;
		}		
	}
    if (TI)
    {

    }
}

/*----------------------------
发送串口数据
----------------------------*/
void Send1Data(BYTE dat)
{
    SBUF = dat;                 //写数据到UART数据寄存器
	while(TI==0);				//等待发送完毕,然后再将发送中断标志清零
	TI = 0;	
}

void Send1Datas(uchar *s,uint len)
{
	uint i=len;
//	BUZZER = !BUZZER;
	if(len==0){
		Send1String(s);
		return;
	}
	while(i){
		Send1Data(*s++);
//		if(*s == '\r' || *s == '\0')
//			break;
		i--;
	}
}

/*----------------------------
发送字符串
----------------------------*/
void Send1String(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        Send1Data(*s++);         //发送当前字符
    }
}

//*-------------------------------------------------------------
//*功能说明:	串口接收函数,使用T3超时判定
//*入口参数:	*Data--接收缓存,要足够大
//*				len--需要接收的长度,0表示接收最大长度
//*				time--超时长度,单位ms
//*返 回 值:	收到的字节数
//*-------------------------------------------------------------
extern unsigned long System1MsCnt;
int Recv1Bytes(unsigned char *Data,int len,unsigned int time)
{
	unsigned int i=0;
	unsigned long curtime = System1MsCnt;
	while(1){
		if(RI == 1){			
			Data[i++] = SBUF;
			RI = 0;
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