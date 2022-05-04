#include "remote.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "oled.h"
#include "led.h"
#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//红外遥控解码驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//红外遥控初始化
//设置IO以及定时器4的输入捕获
void Remote_Init(void)    			  
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能PORTB时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 输入 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//上拉输入 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//初始化GPIOB.9
	
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000; //设定计数器自动重装值 最大10ms溢出  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//预分频器,1M的计数频率,1us加1.	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // 选择输入端 IC4映射到TI4上
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
  TIM_ICInit(TIM4, &TIM_ICInitStructure);//初始化定时器输入捕获通道

  TIM_Cmd(TIM4,ENABLE ); 	//使能定时器4
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	

	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断								 
}

//处理红外键盘
//返回值:
//	 0,没有任何按键按下
//其他,按下的按键键值.
//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	  
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//键值正确	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	}  
    return sta;
}

u8 key_get=0;		//所得键值
char str[12];	
int n=-1;		//红外遥控设置时间位
//static int x=3,y=0;//时间坐标
//int i,j,k=0;
extern u8 Led_Status;//LED灯标志位
extern u8 Windows_Status;//窗户打开标志
extern u8 Door_Status;//门打开标志
extern u8 SR301_Status;//是否有人标志
extern u8 Beep_Status;//警报响标志
extern int position;//OLED“>”的坐标
extern u8 Mq135_Max;//气体阈值
extern u8 Temp_Max;//温度阈值
extern u8 Humi_Max;//湿度阈值
extern u8 Light_Max;//光度阈值
extern u8 mode;
extern u8 ClearOled;//为了解决红外遥控导致的OLED刷屏BUG而设定的标志位
extern u8 HomeMode;//自动模式标志位 0：手动模式 1：自动模式 2:离家模式 3：睡觉模式

void remote_change_time()
{
	key_get=Remote_Scan();	
	if(key_get)
	{	 
		switch(key_get)
		{
			case 0:strcpy(str,"ERROR");break;			   
			case 162:strcpy(str,"POWER");break;	    
			case 98:strcpy(str,"UP");break;	    
			case 2:strcpy(str,"PLAY");break;		 
			case 226:strcpy(str,"ALIENTEK");break;		  
			case 194:strcpy(str,"RIGHT");break;	   
			case 34:strcpy(str,"LEFT");break;		  
			case 224:strcpy(str,"VOL-");break;		  
			case 168:strcpy(str,"DOWN");break;		   
			case 144:strcpy(str,"VOL+");break;		    
			case 104:n=1;break;		  
			case 152:n=2;break;	   
			case 176:n=3;break;	    
			case 48 :n=4;break;		    
			case 24 :n=5;break;		    
			case 122:n=6;break;		  
			case 16 :n=7;break;			   					
			case 56 :n=8;break;	 
			case 90 :n=9;break;
			case 66 :n=0;break;
			case 82 :strcpy(str,"DELETE");break;
			default :strcpy(str," ");break;
		}
		if (n>=0&&n<=9)
			strcpy(str," ");
		if(strcmp(str,"ALIENTEK")==0)
		{
			//DEBUG_LOG("Key=ALIENTEK\r\n");
		}
		else if(strcmp(str,"UP")==0)//往上
		{
			//DEBUG_LOG("Key=UP\r\n");
			position--;
			if(mode==0 || mode==2 || mode==3 || mode==4)
			{
				OLED_ClearPos();
				if(position<0)
					position=3;
			}
			else if(mode==1)
			{
				/*1、设置时间(后期改一下) 2、查看时间*/
				if(position<0)
					position=1;
			}
		}
		else if(strcmp(str,"DOWN")==0)//往下
		{	
			//DEBUG_LOG("Key=DOWN\r\n");
			position++;
			if(mode==0 || mode==2 || mode==3 || mode==4)
			{
				OLED_ClearPos();
				if(position>3)
					position=0;
			}
			else if(mode==1)
			{
				/*1、设置时间(后期改一下) 2、查看时间*/
				if(position>1)
					position=0;
			}
		}
		else if(strcmp(str,"LEFT")==0)//往左
		{
			//DEBUG_LOG("Key=LEFT\r\n");
		}
		else if(strcmp(str,"RIGHT")==0)//往右
		{
			//DEBUG_LOG("Key=RIGHT\r\n");
		}
		else if(strcmp(str,"PLAY")==0)//确认键
		{
			//DEBUG_LOG("Key=PLAY\r\n");
			if(mode==0)
			{
				if(position==0)
					mode=1;
				else if(position==1)
					mode=2;
				else if(position==2)
					mode=3;
				else if(position==3)
					mode=4;
				position=0;
				ClearOled=1;
				//OLED_Clear();
			}
			else if(mode==3)
			{
				if(position==0)
				{
					if(Windows_Status==0)
						Windows_Status=1;
					else
						Windows_Status=0;
					//Windows_Status=!Windows_Status;
				}
				else if(position==1)
				{
					if(Door_Status==0)
					{
						Door_Status=1;
						TIM_SetCompare1(TIM3, 195);//开门
					}
					else
					{
						Door_Status=0;
						TIM_SetCompare1(TIM3, 185);//关门
					}
					//Door_Status=!Door_Status;
				}
				else if(position==2)
				{
					if(Led_Status==0)
					{
						Led_Status=1;
						LED0=1;
					}
					else
					{
						Led_Status=0;
						LED0=0;
					}
					//Led_Status=!Led_Status;
				}
				else if(position==3)
				{
					if(Beep_Status==0)
					{
						Beep_Status=1;
						BEEP=1;
					}
					else
					{
						Beep_Status=0;
						BEEP=0;
					}
					//Beep_Status=!Beep_Status;
				}
			}
			else if(mode==4)
			{
				if(position==0)
					HomeMode=0;
				else if(position==1)
					HomeMode=1;
				else if(position==2)
					HomeMode=2;
				else if(position==3)
					HomeMode=3;
			}
		}
		else if(strcmp(str,"VOL+")==0)//增加键
		{
			//DEBUG_LOG("Key=VOL+\r\n");
			if(mode==2)
			{
				if(position==0)
					Mq135_Max++;
				else if(position==1)
					Temp_Max++;
				else if(position==2)
					Humi_Max++;
				else if(position==3)
					Light_Max++;
			}
		}
		else if(strcmp(str,"VOL-")==0)//减少键
		{
			//DEBUG_LOG("Key=VOL-\r\n");
			if(mode==2)
			{
				if(position==0)
					Mq135_Max--;
				else if(position==1)
					Temp_Max--;
				else if(position==2)
					Humi_Max--;
				else if(position==3)
					Light_Max--;
			}
		}
		else if(strcmp(str,"DELETE")==0)//返回键
		{
			//DEBUG_LOG("Key=BACK\r\n");
			if(mode==1 || mode==2 || mode==3 || mode==4)
			{
				mode=0;
				position=0;
				ClearOled=1;
				//OLED_Clear();
			}
		}	
		else if (n!=-1)
		{ 
			n=-1;
		}
	}
}

//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    	 
 
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								//上次有数据被接收到了
		{	
			RmtSta&=~0X10;							//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)
			{
				RmtSta&=~(1<<7);//清空引导标识(按一次按键只能发送一次信息)
				RmtSta|=1<<6;//标记已经完成一次按键的键值信息采集
			}
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					//清空引导标识
				RmtSta&=0XF0;						//清空计数器	
			}								 	   	
		}							    
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{	  
		if(RDATA)//上升沿捕获
		{
  			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);						//CC4P=1	设置为下降沿捕获
			TIM_SetCounter(TIM4,0);							//清空定时器值
			RmtSta|=0X10;							//标记上升沿已经被捕获
		}else //下降沿捕获
		{
			Dval=TIM_GetCapture4(TIM4);					//读取CCR4也可以清CC4IF标志位
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);				//CC4P=0	设置为上升沿捕获
			if(RmtSta&0X10)							//完成一次高电平捕获 
			{
 				if(RmtSta&0X80)//接收到了引导码
				{
					
					if(Dval>300&&Dval<800)			//560为标准值,560us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=0;					//接收到0	   
					}else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=1;					//接收到1
					}else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					{
						RmtCnt++; 					//按键次数增加1次
						RmtSta&=0XF0;				//清空计时器		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms
				{
					RmtSta|=1<<7;					//标记成功接收到了引导码
					RmtCnt=0;						//清除按键次数计数器
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);
	remote_change_time();	
}


































