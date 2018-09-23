/**====================================================================*
*							DHT11驱动函数
*----------------------------------------------------------------------*
*文 件 名:    DHT11.c
*文件描述:    读取DHT11温湿度传感器的数据
*软件版本:    V1.1
*硬件环境:    
*创 建 人:    方规  526293582@qq.com(邮箱) 13477037750(手机)
*最后修改:    2017/06/09 16:33
*功能说明:    
-----------------------------------------------------------------------*
*DHT11使用方法:
1.	起始信号:单片机拉低数据线19ms,然后拉高数据线,含义"告诉DHT11我需要数据了"
2.	应答信号:DHT11收到起始信号后,将总线拉低80us,然后再拉高80us,含义"我准备好
	发送数据了",接下来就是数据信号了
3.	数据信号:每一bit数据都以50us低电平时隙开始,高电平的长短定了数据位是0(26us至28us)
	还是1(70us),当最后一bit数据传送完毕后，DHT11拉低总线50us,随后总线由上拉电阻拉高
	进入空闲状态
=======================================================================*/

#include "DHT11.h"
#include "Uart0.h"

//
//温湿度定义
uchar ucharFLAG=0,uchartemp=0;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;


//温湿度传感
void COM(void)    // 温湿写入
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);	//50us低电平,表示信号的开始,随后跳过信号0(至少28us)的延时,然后检测数据线的高低
		DelayUs(30);						//***逻辑分析仪测试,大约延时53us,此函数延时不精确,开发是需要用工具测试!!
        uchartemp=0;
        if(DATA_PIN){
			uchartemp=1;	
		}else{
		}
        
		ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);		//最后,如果数据线持续高电平很长时间后,数据线仍然为高,那么说明数据传输结束了(由上拉电阻拉高的)

		if(ucharFLAG==1){
			break;    
		}
        
		ucharcomdata<<=1;
        ucharcomdata|=uchartemp; 
    }    
}

unsigned int DHT11(void)   	//温湿传感启动
{
	SetPinMode(P1M0,P1M1,POMOD,1);			//设置P1.1为输出
	
    DATA_PIN=0;
    DelayMs(19);  			//>18MS
    DATA_PIN=1; 

	SetPinMode(P1M0,P1M1,INMOD,1);			//设置P1.1为输入
	
    DelayUs(10);
    DelayUs(10);                        
    DelayUs(10);
    DelayUs(10);  
	
    if(!DATA_PIN) 
    {		
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);		//DHT11拉低80us响应主机信号
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++); 		//DHT11拉高80us响应主机信号,后面的就是数据信号了
        COM();
        ucharRH_data_H_temp=ucharcomdata;		//湿度高八位
        COM();
        ucharRH_data_L_temp=ucharcomdata;		//湿度低八位
        COM();
        ucharT_data_H_temp=ucharcomdata;		//温度高八位
        COM();
        ucharT_data_L_temp=ucharcomdata;		//温度低八位
        COM();
        ucharcheckdata_temp=ucharcomdata;		//校验位
        DATA_PIN=1; 
		
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
		
        if(uchartemp==ucharcheckdata_temp)
        {
            ucharRH_data_H=ucharRH_data_H_temp;
            ucharRH_data_L=ucharRH_data_L_temp;
            ucharT_data_H=ucharT_data_H_temp;
            ucharT_data_L=ucharT_data_L_temp;
            ucharcheckdata=ucharcheckdata_temp;
        }    
    }	
//	Send0Data(ucharT_data_H<<8);Send0Data(ucharRH_data_H<<0);
	return (ucharT_data_H<<8)|(ucharRH_data_H<<0);
}
