/**====================================================================*
*					16*16 点阵串口改字实验
*----------------------------------------------------------------------*
*文 件 名:    main.c
*文件描述:    
*软件版本:    V1.0
*硬件环境:    WZ01_WN_A_V1.0
*创 建 人:    方规  526293582@qq.com(邮箱) 13477037750(手机)
*最后修改:    2017/06/11 15:29
*功能说明:    点阵循环显示串口0接收到的数据(汉字或字符)
1.	串口修改点阵屏的内容时,需要等待上显示结束(到下次显示之间的空隙时间),串口
	接收和点阵屏显示都是阻塞的方式,所以在显示的时候不能接收新内容
2.	可以调整串口接收超时(入口参数3),来增加串口阻塞时间
3.	串口初始化后将打印当前支持的汉字字库(ASC字符,支持所有)
4.  添加键值控制点阵显示不同的内容
*待 解 决:
1.	串口修改内容时,如果奇数个字符后面跟着汉字,则显示异常,如"唯abc众",遇到这种
	情况直接在字符后面添加空格凑齐即可正常显示
-----------------------------------------------------------------------*
*注意事项:
1.	STC15W4K32S4系列的芯片,上电后所有与PWM相关的IO口均为高阻态,需将这些口设置
	为准双向口或强推挽模式方可正常使用
	相关IO: P0.6/P0.7/P1.6/P1.7/P2.1/P2.2 P2.3/P2.7/P3.7/P4.2/P4.4/P4.5
2.	STC15单片机使用PxM0和PxM1两个寄存器控制IO口模式(0:弱上拉 1:强推挽 2:输入 3:开漏)
3.	串口0可以使用P3.0/P3.1 P3.6/P3.7 P1.6/P1.7可以使用UART0_PORT宏来选择
4.  使用创意APP键值1或键值2控制点阵显示的内容时,必须在点阵显示完毕后才能按下一个键值
=======================================================================*/
#include "WZ01_SN_A_V1.0.h"	
#include "Uart1.h"
#include "Uart3.h"
#include "Max7219.h"

ulong System1MsCnt = 0;				//系统1ms计数器
ulong SystemSecond = 0;

void Timer0Init()
{
    AUXR |= 0x80;                   //定时器0为1T模式
//  AUXR &= 0x7f;                   //定时器0为12T模式

    TMOD = 0x00;                    //设置定时器为模式0(16位自动重装载)
    TL0 = T1MS;                     //初始化计时值
    TH0 = T1MS >> 8;
    TR0 = 1;                        //定时器0开始计时
    ET0 = 1;                        //使能定时器0中断
    EA = 1;
}
//*==============================================|=====================*
//*函数原型:    
//*功能描述:    定时器0中断服务函数(启动后每1MS进入一次)
//*==============================================|=====================*
void Timer0_ISR() interrupt 1 using 1
{
    static uint counter0=0;
	counter0++;
	System1MsCnt++;
	
	if(System1MsCnt%1000==0){			
//		BUZZER = !BUZZER;
	}
}


void main(void)
{


	int len=0;
	int temp=0;
	//初始化蜂鸣器
	BUZZER_INIT();
	//时间初始化
	Timer0Init();
	//定时器1的波特率
	Uart1Init();
	//定时器3的波特率
	Uart3Init();

	Send1String("支持汉字库:\r\n");
	Send1String(Chinese_text_16x16);
	
	Init_MAX7219();
	
	OledInit();	
	OledClear();
	OledDispString(1,1,"  点阵显示实验  ");
	fg_LedDisplay("荆州职业技术学院    ");
	BUZZER_ON();
	DelayMs(2000);
	BUZZER_OFF();
	
      while(1){
			
		//1.按键控制显示内容	
		if(KEY1==0)
		{
			DelayMs(10);
			if(KEY1==0)
			{
				Send1String("KEY ");
				temp ++;			
				OledClearHalf(2);
				//当键值为偶数时，除以2等于0，
				if(temp%2==0)
				{								 
			     	OledSprintf(3,1,"技能大赛");
		        	fg_LedDisplay("这是按键2  ");
				    LED2_OFF();
					BUZZER_ON();
					DelayMs(100);
					BUZZER_OFF();
				
				}
				else
				{
				    fg_LedDisplay();
								 
					OledSprintf(3,1,"技能大赛");
					fg_LedDisplay("这是按键1  ");
					LED2_ON();
					BUZZER_ON();
					DelayMs(300);
					BUZZER_OFF();
				}			
			}
		}



		//2.网关变量控制显示内容
		if(gUart3_RecvFlag==1)	//判断串口是否收到命令字符串
		{
		    Send3String(gUart3_RecvBuf+1);
		
			OledClearHalf(2);
			if(strncmp(gUart3_RecvBuf+1,"#key 1",6)==0)//键值方式控制显示内容
			{							
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();	   
				OledSprintf(3,1,"红灯");
				fg_LedDisplay("红灯禁止通行");
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();
			
			}
			else if(strncmp(gUart3_RecvBuf+1,"#key 2",6)==0)
			{
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();
				OledSprintf(3,1,"绿灯");
				fg_LedDisplay("绿灯允许通行");
				BUZZER_ON();
				DelayMs(100);
				BUZZER_OFF();
		
			}
		memset(gUart3_RecvBuf,0,SIZEOF(gUart3_RecvBuf));
		gUart3_RecvFlag=0;

	}
	  


	
		

		
		
		
		 

		//3.调试串口修改显示内容,阻塞的形式,每次主循环将占用CPU时间,会导致主机变量控制不灵
		if(gUart1_RecvFlag==1)																	  
		{
			fg_LedDisplay(gUart1_RecvBuf+1);
			memset(gUart1_RecvBuf,0,SIZEOF(gUart1_RecvBuf));
			gUart1_RecvFlag = 0;		
		}
	}
}
