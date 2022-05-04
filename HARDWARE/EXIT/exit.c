#include "exit.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "beep.h"
#include "time.h"
#include "touch.h"
#include "lcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
void EXTIX_Init(void)
{
 
   	EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
		GPIO_InitTypeDef  GPIO_InitStructure;
		
		KEY_Init();
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //使能PB,PF端口时钟
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

		//触摸屏PEN口(PF10)初始化
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PF10端口配置
//	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //上拉输入
//	 	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10上拉输入
//	 	GPIO_SetBits(GPIOF,GPIO_Pin_10);//上拉	

//   //GPIOF.10	  中断线以及中断初始化配置 下降沿触发
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource10);
//  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//	
//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键WK_UP所在的外部中断通道
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级1， 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//子优先级0
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//  	NVIC_Init(&NVIC_InitStructure); 
	  
   //GPIOE.3	  中断线以及中断初始化配置 下降沿触发 //KEY1
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

   //GPIOE.4	  中断线以及中断初始化配置  下降沿触发	//KEY0
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


   //GPIOA.0	  中断线以及中断初始化配置 上升沿触发 PA0  WK_UP
 	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 

  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键WK_UP所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//使能按键KEY1所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能按键KEY0所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
}

u8 flag=0;
extern u8 leaveflag;//离家标志位 0：在家 1：离家
extern u8 Led_Status;//LED灯标志位
extern u8 Windows_Status;//窗户打开标志
extern u8 Door_Status;//门打开标志
extern u8 SR301_Status;//是否有人标志
extern u8 Beep_Status;//警报响标志
extern int Lcd_Page;//LCD功能页面
extern float Mq135_Max;//气体阈值
extern u8 Temp_Max;//温度最大阈值
extern u8 Humi_Max;//湿度最大阈值
extern u8 Light_Max;//光度阈值
extern u8 Music_Voice;//音乐音量
extern u8 Play_flag;//0:暂停 1：播放
extern u8 HomeMode;//0：手动模式 1：自动模式 2:离家模式 3：电影模式
extern int Clock_Num;//闹钟数量，目前设置最多可设置3个闹钟
extern int AlarmClock[3][3];//闹钟，初步规定最多可设置3个闹钟精确到分,第3位为选中闹钟标志位;第0：时，第1：分，第2：选中标志
extern u8 changeflag;//页面切换清屏标志位
extern u8 clockclear[4];//设置闹钟被选中/取消/删除后的清除特定区域的标志位，1代表点击，0代表每点击；
//clockclear[4] 第1~3位为点击了闹钟选中按钮的标志位，第4位为点击了删除的标志位
extern u8 musicclear[2];//音乐控制界面清除标志位;第1位：标志音量条的清除标志 第2位：标志播放/暂停清除标志
u8 count=0;

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(WK_UP==1)	 	 //WK_UP按键
	{			
		if(leaveflag==1)
			leaveflag=0;//清楚离家标志位
		if(Windows_Status==0)
			Windows_Status=1;
		else 
			Windows_Status=0;
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
}
 
u8 stop_flag=0;
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY1==0)	 //按键KEY1
	{
		if(Door_Status==1)
		{
			Door_Status=0;
			TIM_SetCompare1(TIM3, 185);//开门	 
		}
		else if(Door_Status==0)
		{
			Door_Status=1;
			TIM_SetCompare1(TIM3, 195);//开门	 
		}
	}	
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY0==0)	 //按键KEY0
	{
		if(Led_Status==1)
		{
			Led_Status=0;
			LED0=0;//开灯	 
		}
		else if(Led_Status==0)
		{
			Led_Status=1;
			LED0=1;//关灯	 
		}
	}		 
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}
 
//触摸屏中断
//外部中断10服务程序 
void EXTI15_10_IRQHandler(void)
{
	DEBUG_LOG("进入中断成功");
	delay_ms(10);//防抖
	if(EXTI_GetITStatus(EXTI_Line10) !=RESET)
	{
		count++;//测试点击一下触摸屏会进入几次中断
		DEBUG_LOG("进入中断次数：%d\r\n",count);
		if(count>=10)
			count=0;
		//LCD_Clear(WHITE);
		EXTI_ClearITPendingBit(EXTI_Line10);  //清除LINE4上的中断标志位
		tp_dev.sta|=1<<7;//标记触摸屏被按下
		if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))
		{
			tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff; 
		}
		DEBUG_LOG("中断成功，触摸坐标为：x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
//		if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>96&&tp_dev.y[0]<144)
//		{
//			LCD_Clear(WHITE);
//			Lcd_Page--;//LCD功能页面
//			if(Lcd_Page<0)
//				Lcd_Page=4;
//		}
//		if(tp_dev.x[0]>192&&tp_dev.x[0]<240 && tp_dev.y[0]>96&&tp_dev.y[0]<144)
//		{
//			LCD_Clear(WHITE);
//			Lcd_Page++;//LCD功能页面
//			if(Lcd_Page>4)
//				Lcd_Page=0;
//		}
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{
			//LCD_Clear(WHITE);
			//tp_dev.sta&=~(1<<7);//标记按键松开	
			if(Lcd_Page>=0 && Lcd_Page<=5)//只有在切换功能页面的时候可以起作用（目前一共有5个模块）
			{
				changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//如果按下左键
				{
					//LCD_Clear(WHITE);
					Lcd_Page--;//LCD功能页面
					if(Lcd_Page<0)
						Lcd_Page=5;
				}
				else if(tp_dev.x[0]>192&&tp_dev.x[0]<240 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//如果按下右键
				{
					//LCD_Clear(WHITE);
					Lcd_Page++;//LCD功能页面
					if(Lcd_Page>5)
						Lcd_Page=0;
				}
				else if(tp_dev.x[0]>56&&tp_dev.x[0]<184 && tp_dev.y[0]>96&&tp_dev.y[0]<224)//如果选中对应功能页面
				{
					if(Lcd_Page==0)
					{
						//LCD_Clear(WHITE);清屏函数放在中断不起作用
						Lcd_Page=6;
					}
					else if(Lcd_Page==1)
					{
						//LCD_Clear(LIGHTBLUE);
						Lcd_Page=7;
					}
					else if(Lcd_Page==2)
					{
						//LCD_Clear(WHITE);
						Lcd_Page=8;
					}
					else if(Lcd_Page==3)
					{
						//LCD_Clear(WHITE);
						Lcd_Page=9;
					}
					else if(Lcd_Page==4)
					{
						//LCD_Clear(WHITE);
						Lcd_Page=10;
					}
					else if(Lcd_Page==5)
					{
						//LCD_Clear(WHITE);
						Lcd_Page=11;
					}
				}
			}
			if(Lcd_Page==8)//设置页面的阈值加减
			{
				if(tp_dev.x[0]>160&&tp_dev.x[0]<180 && tp_dev.y[0]>64&&tp_dev.y[0]<88)
					Mq135_Max++;
				else if(tp_dev.x[0]>180&&tp_dev.x[0]<200 && tp_dev.y[0]>64&&tp_dev.y[0]<88)
					Mq135_Max--;
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<180 && tp_dev.y[0]>96&&tp_dev.y[0]<120)
					Temp_Max++;
				else if(tp_dev.x[0]>180&&tp_dev.x[0]<200 && tp_dev.y[0]>96&&tp_dev.y[0]<120)
					Temp_Max--;
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<180 && tp_dev.y[0]>128&&tp_dev.y[0]<152)
					Humi_Max++;
				else if(tp_dev.x[0]>180&&tp_dev.x[0]<200 && tp_dev.y[0]>128&&tp_dev.y[0]<152)
					Humi_Max--;
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<180 && tp_dev.y[0]>160&&tp_dev.y[0]<184)
					Light_Max++;
				else if(tp_dev.x[0]>180&&tp_dev.x[0]<200 && tp_dev.y[0]>160&&tp_dev.y[0]<184)
					Light_Max--;
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)
				{
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=2;
				}
			}
			else if(Lcd_Page==9)
			{
				if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>64&&tp_dev.y[0]<96)//点击开窗按钮
				{
					if(Windows_Status==0)
						Windows_Status=1;
					else
						Windows_Status=0;
				}
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>96&&tp_dev.y[0]<128)//点击开门按钮
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
				}
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>128&&tp_dev.y[0]<160)//点击LED按钮
				{
					if(Led_Status==1)
					{
						Led_Status=0;
						LED0=0;
					}
					else
					{
						Led_Status=1;
						LED0=1;
					}
				}				
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>160&&tp_dev.y[0]<192)//点击蜂鸣器按钮
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
				}				
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回主页面
				{
					//LCD_Clear(WHITE);
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=3;
				}
			}
			else if(Lcd_Page==7)//音乐播放界面
			{
				if(tp_dev.x[0]>24&&tp_dev.x[0]<56 && tp_dev.y[0]>16&&tp_dev.y[0]<48)//减音量
				{
					if(Music_Voice>=2)//此处出现一个BUG,当判断条件为if(Music_Voice>=1)时，当音量减到0时界面会出现绿色闪屏
					{
						musicclear[0]=1;//音量条清色标志位
						//LCD_Fill(58,26,54+Music_Voice*4,38,WHITE);//音量条清色
						Music_Voice--;
						//此行后期需增加通过串口向MP3模块发送指令
					}
				}
				else if(tp_dev.x[0]>176&&tp_dev.x[0]<208 && tp_dev.y[0]>16&&tp_dev.y[0]<48)//加音量
				{
					if(Music_Voice<=29)
					{
						musicclear[0]=1;//音量条清色标志位
						//LCD_Fill(58,26,54+Music_Voice*4,38,WHITE);//音量条清色
						Music_Voice++;
						//此行后期需增加通过串口向MP3模块发送指令
					}
				}
				else if(tp_dev.x[0]>90&&tp_dev.x[0]<150 && tp_dev.y[0]>216&&tp_dev.y[0]<276)//播放/暂停按键切换
				{
					musicclear[1]=1;//播放/停止按键清色标志位
					if(Play_flag==0)
					{ 
						//LCD_Fill(90,216,150,276,WHITE);//按键条清色
						Play_flag=1;//播放音乐，显示播放图标(在lcd.c里面进行切换)
					}
					else
					{
						//LCD_Fill(90,216,150,276,WHITE);//按键条清色
						Play_flag=0;//暂停播放，显示暂停图标(在lcd.c里面进行切换)
					}
				}
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回主页面
				{
					//LCD_Clear(WHITE);
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=1;
				}
			}
			else if(Lcd_Page==10)//模式选择操作界面
			{
				if(tp_dev.x[0]>8&&tp_dev.x[0]<112 && tp_dev.y[0]>92&&tp_dev.y[0]<152)//mode0:手动模式
					HomeMode=0;
				else if(tp_dev.x[0]>128&&tp_dev.x[0]<232 && tp_dev.y[0]>92&&tp_dev.y[0]<152)//mode1:自动模式
					HomeMode=1;
				else if(tp_dev.x[0]>8&&tp_dev.x[0]<112 && tp_dev.y[0]>168&&tp_dev.y[0]<228)//mode2:离家模式
					HomeMode=2;
				else if(tp_dev.x[0]>128&&tp_dev.x[0]<232 && tp_dev.y[0]>168&&tp_dev.y[0]<228)//mode3:电影模式
					HomeMode=3;
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回主页面
				{
					//LCD_Clear(WHITE);
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=4;
				}
			}
			else if(Lcd_Page==6)//闹钟设置界面
			{
//				if(tp_dev.x[0]>16&&tp_dev.x[0]<48 && tp_dev.y[0]>16&&tp_dev.y[0]<38)//时增
//				{
//					AlarmClock[0][0]++;
//					if(AlarmClock[0][0]>23)
//						AlarmClock[0][0]=0;
//				}
//				else if(tp_dev.x[0]>16&&tp_dev.x[0]<48 && tp_dev.y[0]>66&&tp_dev.y[0]<88)//时减
//				{
//					AlarmClock[0][0]--;
//					if(AlarmClock[0][0]<0)
//						AlarmClock[0][0]=23;
//				}
//				else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>16&&tp_dev.y[0]<38)//分增
//				{
//					AlarmClock[0][1]++;
//					if(AlarmClock[0][1]>59)
//						AlarmClock[0][1]=0;
//				}
//				else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>66&&tp_dev.y[0]<88)//分减
//				{
//					AlarmClock[0][1]--;
//					if(AlarmClock[0][1]<0)
//						AlarmClock[0][1]=59;
//				}
				//点击选中启动闹钟按钮(并且存在待设置的闹钟)加tp_dev.y[0]<272条件是防止和添加时钟符号冲突
				if(tp_dev.x[0]>16 && tp_dev.x[0]<228 && tp_dev.y[0]>16 && tp_dev.y[0]<272 && Clock_Num>0)
				{
					int k=0;
					for(k=0;k<Clock_Num;k++)
					{
						if(tp_dev.x[0]<180 && tp_dev.y[0]>16 && tp_dev.y[0]<248)//如果点击的是加减时间区域
						{
							if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//时增
							{
								AlarmClock[k][0]++;
								if(AlarmClock[k][0]>23)
									AlarmClock[k][0]=0;
							}
							else if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(66+k*80) && tp_dev.y[0]<(88+k*80))//时减
							{
								AlarmClock[k][0]--;
								if(AlarmClock[k][0]<0)
									AlarmClock[k][0]=23;
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//分增
							{
								AlarmClock[k][1]++;
								if(AlarmClock[k][1]>59)
									AlarmClock[k][1]=0;
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(66+k*80) &&tp_dev.y[0]<(88+k*80))//分减
							{
								AlarmClock[k][1]--;
								if(AlarmClock[k][1]<0)
									AlarmClock[k][1]=59;
							}
						}
						else if(tp_dev.x[0]>180 && tp_dev.y[0]>(28+k*80) && tp_dev.y[0]<(76+k*80))//如果启动了闹钟k+1(k+1为闹钟1~3),如果点击的是选定按钮区域
						{
							clockclear[k]=1;//给lcd.c标记点击了对应闹钟按钮，lcd.c里面的显示函数进行对应的区域的清除
							if(AlarmClock[k][2]==0)
							{
								//LCD_Fill(180,28+k*80,228,76+k*80,WHITE);//把前一次的按钮状态清掉
								AlarmClock[k][2]=1;
							}
							else
							{
								//LCD_Fill(180,28+k*80,228,76+k*80,WHITE);//把前一次的按钮状态清掉
								AlarmClock[k][2]=0;
							}
						}
					}
				} 
				else if(tp_dev.y[0]>272&&tp_dev.y[0]<320)//点击添加/删除闹钟按钮
				{
					if(tp_dev.x[0]>120&&tp_dev.x[0]<168)//按下的是删除按钮
					{
						//int j=0;
						Clock_Num--;
						if(Clock_Num<0)//防止越界
							Clock_Num=0;
						clockclear[3]=1;//给lcd.c标记点击了对应闹钟按钮，lcd.c里面的显示函数进行对应的区域的清除
						//LCD_Fill(0,16+Clock_Num*80,240,88+Clock_Num*80,WHITE);//删除后需要消除的闹钟记录（清除画点数据）
						AlarmClock[Clock_Num][0]=0;
						AlarmClock[Clock_Num][1]=0;
						AlarmClock[Clock_Num][2]=0;//删除闹钟后同样要删除其时间及选中标志位记录
					}
					else if(tp_dev.x[0]>184&&tp_dev.x[0]<232)//按下的是添加按钮
					{
						Clock_Num++;
						if(Clock_Num>3)//防止闹钟数越界，初设最多设置3个闹钟
							Clock_Num=3;
					}
					else if(tp_dev.x[0]>0&&tp_dev.x[0]<48)//返回进入前的页面，因为放在了闹钟添加/删除同一行，所以代码放在此处同讨论
					{
						//LCD_Clear(WHITE);
						changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
						Lcd_Page=0;
					}
				}
//				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回进入前的页面
//				{
//					LCD_Clear(WHITE);
//					Lcd_Page=0;
//				}
			}
			else if(Lcd_Page==11)//天气时钟显示界面
			{
				  if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回进入前的页面
				{
					//LCD_Clear(WHITE);
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=5;
				}
			}
		} 
	}
	EXTI_ClearITPendingBit(EXTI_Line10);  //清除LINE4上的中断标志位
}

