#include "timer.h"
#include "sys.h"
#include "led.h"
#include "beep.h"
#include "oled.h"
#include "adc.h"
#include "lsens.h"
#include "dht11.h" 
#include "stdio.h"
#include "math.h"
#include "mq135.h"
#include "usart.h"
#include "lcd.h"
#include "touch.h"
#include "rtc.h"

u8 temperatureH,temperatureL,humidityH,humidityL;
extern uint8_t num;
extern u16 adc_data;
extern float adc_vol;
extern u16 mq135_data;
extern float mq135_vol;
u8 light;
char mq_ppm[20];
char Light_val[20];
extern char str[12];	
extern char temp[20];
extern char humi[20];
extern char ADC_data[20];
extern char ADC_vol[20];
extern char MQ135_data[20];
extern char MQ135_vol[20];
extern int position;//OLED“>”的坐标
extern float mq135_ppm;
extern float Light;
extern u8 Temp_Max,Humi_Max,Temp_Min,Humi_Min;//温度阈值,湿度阈值,
extern float Mq135_Max;//空气质量阈值
extern u8 Windows_Status;//窗户打开标志
extern u8 lastmove;//上一次窗户打开情况，默认初始状态为关 1：开 0：关
extern u8 voice_command;//语音识别指令
extern u8 HomeMode;//自动模式标志位 0：手动模式 1：自动模式 2:离家模式 3：睡觉模式

//以下标志位用于发送短信的警报标志
extern u8 Temp_message;
extern u8 Temp_lastflag;//上一次温度的标志，低于阈值为0，大于阈值为1
extern u8 Temp_nowflag;//当前温度的标志
extern u8 Humi_message;
extern u8 Humi_lastflag;//上一次湿度的标志，低于阈值为0，大于阈值为1
extern u8 Humi_nowflag;//当前湿度的标志
extern u8 MQ135_message;
extern u8 MQ135_lastflag;//上一次空气质量的标志，低于阈值为0，大于阈值为1
extern u8 MQ135_nowflag;//当前空气质量的标志
extern u8 mode;
extern u8 changeflag;
extern int Lcd_Page;
extern vu16 USART1_RX_STA;
//定时器3初始化函数
void Timer3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	// NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM_Cmd(TIM3,ENABLE);
}

//定时器5初始化函数
void Timer5_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	// NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM_Cmd(TIM5,DISABLE);
}

/********************************定时器3中断处理函数**************************************/
//函数功能：读取传感器数据，并根据光强控制电机的开关(模拟窗户的开关)
u8 TIM3_count1=0;
u16 TIM3_count2=0;
extern u8 OK;//主函数进入循环标志位
extern u8 AlarmClock[3][3];//闹钟，初步规定最多可设置3个闹钟精确到分,第3位为选中闹钟标志位;第0：时，第1：分，第2：选中标志
extern u8 leaveflag;//离家标志位 0：在家 1：离家
extern u8 Led_Status;//LED灯标志位
extern u8 Door_Status;//门打开标志
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		TIM3_count1++;
		if(TIM3_count1>=25)
		{
			if(temperatureH>=Temp_Max)
				Temp_lastflag=1;
			else
				Temp_lastflag=0;
			if(humidityH>=Humi_Max)
				Humi_lastflag=1;
			else
				Humi_lastflag=0;
			if(mq135_ppm>=3000)
				MQ135_lastflag=1;
			else
				MQ135_lastflag=0;
			TIM3_count1=0;
			DHT11_Read_Data(&temperatureH,&temperatureL,&humidityH,&humidityL);	//读取温湿度值	
			Light=Lsens_Get_Val();//获取光照强度
			adc_data=Get_Adc_Average(ADC_Channel_1,10);
			adc_vol=(float)adc_data*(3.3/4096);
			mq135_data=Get_MA135_Adc_Average(ADC_Channel_6,10);
			mq135_vol=(float)mq135_data*(3.3/4096);
			mq135_ppm=pow((3.4880*10*mq135_vol)/(5-mq135_vol),(1.0/0.3203));
			if(temperatureH>=Temp_Max)
				Temp_nowflag=1;
			else
				Temp_nowflag=0;
			if(humidityH>=Humi_Max)
				Humi_nowflag=1;
			else
				Humi_nowflag=0;
			if(mq135_ppm>=3000)
				MQ135_nowflag=1;
			else
				MQ135_nowflag=0;
			if(Temp_lastflag==0 && Temp_nowflag==1)
				Temp_message=1;
			if(Humi_lastflag==0 && Humi_nowflag==1)
				Humi_message=1;
			if(MQ135_lastflag==0 && MQ135_nowflag==1)
				MQ135_message=1;
			
			if(AlarmClock[0][2]==1 || AlarmClock[1][2]==1 || AlarmClock[2][2]==1)
			{
				if(AlarmClock[0][2]==1)
				{
					//DEBUG_LOG("闹钟1被选中");
					if(AlarmClock[0][0]==calendar.hour && AlarmClock[0][1]==calendar.min && calendar.sec<15)
					{
						//DEBUG_LOG("闹钟1响铃");
						BEEP=!BEEP;
						LED1=!LED1;
					}
					else if(AlarmClock[0][0]==calendar.hour && AlarmClock[0][1]==calendar.min && calendar.sec>15)
					{
						BEEP=0;
						LED1=1;
					}
				}
				if(AlarmClock[1][2]==1)
				{
					//DEBUG_LOG("闹钟2被选中");
					if(AlarmClock[1][0]==calendar.hour && AlarmClock[1][1]==calendar.min && calendar.sec<15)
					{
						//DEBUG_LOG("闹钟2响铃");
						BEEP=!BEEP;
						LED1=!LED1;
					}
					else if(AlarmClock[1][0]==calendar.hour && AlarmClock[1][1]==calendar.min && calendar.sec>15)
					{
						BEEP=0;
						LED1=1;
					}
				}
				if(AlarmClock[2][2]==1)
				{
					//DEBUG_LOG("闹钟3被选中");
					if(AlarmClock[2][0]==calendar.hour && AlarmClock[2][1]==calendar.min && calendar.sec<15)
					{
						//DEBUG_LOG("闹钟3响铃");
						BEEP=!BEEP;
						LED1=!LED1;
					}
					else if(AlarmClock[2][0]==calendar.hour && AlarmClock[2][1]==calendar.min && calendar.sec>15)
					{
						BEEP=0;
						LED1=1;
					}
				}
			}
		}
//		if(HomeMode==0)//手动模式
//		{
//			if(Windows_Status==1)
//			{
//				if(lastmove==0)//如果上一次窗户状态为关，则开窗
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=1;
//					//voice_command=0;
//				}
//			}
//			else if(Windows_Status==0)
//			{
//				if(lastmove==1)//如果上一次窗户状态为开，则关窗
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=0;
//					//voice_command=0;
//				}
//			}
//		}
//		else if(HomeMode==1)//自动模式
//		{
//			if(Lsens_Get_Val()>80)//如果光强>70 过亮
//			{
//				if(lastmove==1)//如果上一次窗户状态为开，则关窗
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=0;
//					//voice_command=0;
//				}
//			}
//			else if(Lsens_Get_Val()<20 || mq135_ppm>500)//如果光强<30  过暗
//			{
//				if(lastmove==0)//如果上一次窗户状态为关，则开窗
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=1;
//					//voice_command=0;
//				}
//			}	
//			//if(temperatureH>Temp_Max || humidityH>Humi_Max || mq135_ppm>Mq135_Max)//阈值报警mq135_ppm存在问题，暂时不需要
//			if(temperatureH>Temp_Max || humidityH>Humi_Max)//阈值报警
//			{
//				LED1=!LED1;
//				BEEP=!BEEP;
//				if(mq135_ppm>Mq135_Max)
//				{
//					if(lastmove==0)//如果上一次窗户状态为关，则开窗
//					{
//						TIM_Cmd(TIM2, ENABLE);
//						Windows_Status=1;
//						//voice_command=0;
//					}
//				}
//			}
//			else
//			{
//				LED1=1;
//				BEEP=0;
//			}
//			TIM_SetCompare2(TIM3,Lsens_Get_Val()*2);//PC7端口控制的LED随光敏传感器的变化而进行PWM调光
//		}
//		else if(HomeMode==2)//离家模式
//		{
//			if(leaveflag==1)
//			{
//				BEEP=!BEEP;//警报有人闯入
//				LED1=!LED1;
//			}
//			else
//			{
//				BEEP=0;
//				LED1=1;
//			}
//			if(lastmove==1)//如果上一次窗户状态为开，则关窗
//			{
//				TIM_Cmd(TIM2, ENABLE);
//				Windows_Status=0;
//				//voice_command=0;
//			}
//			Led_Status=1;//关灯
//			LED0=1;
//			Door_Status=0;//关门
//			TIM_SetCompare1(TIM3, 185);
//		}
//		else if(HomeMode==3)//电影模式
//		{
//			if(lastmove==1)//如果上一次窗户状态为开，则关窗
//			{
//				TIM_Cmd(TIM2, ENABLE);
//				Windows_Status=0;
//				//voice_command=0;
//			}
//			Led_Status=1;//关灯
//			LED0=1;
//		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除终端标志位，为下次中断做准备
	}
}

//u8 count5=0;//给旋转太空人计数
//void TIM5_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM5,TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);//清除终端标志位，为下次中断做准备
////		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
////		{
////			tp_dev.sta&=~(1<<7);//标记按键松开	
////			if(changeflag==1)//如果出现页面切换情况
////			{
////				changeflag=0;
////				LCD_Clear(WHITE);
////			}
////		}
//    //LCD_ShowPage(Lcd_Page);	
//		//LCD_ShowPage(6);	
//		//OLED_Show_mode(mode);
//		LCD_Fill(120,80,240,208,WHITE);
//		LCD_ShowPicture(120,80,114,128,skyman[count5]);//逐帧显示太空人
//		count5++;
//		if(count5>47)
//			count5=0;
//	}
//}
/*************************************************************************************/
//LED呼吸灯：PC7
//舵机：PC6
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;   //声明一个结构体变量，用来初始化GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//声明一个结构体变量，用来初始化定时器

	TIM_OCInitTypeDef TIM_OCInitStructure;//根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	//舵机引脚设置
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;// PC6
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	//LED引脚设置
	//设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOC.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO
	
	//TIM3定时器初始化
	TIM_TimeBaseInitStructure.TIM_Period = arr; //PWM 频率=72000/(199+1)=36Khz//设置自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;//设置用来作为TIMx时钟频率预分频值
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//改变指定管脚的映射	//pC6

	//PWM初始化	  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;

	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	//注意此处初始化时TIM_OC1Init而不是TIM_OCInit，否则会出错。因为固件库的版本不一样。
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR1上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	
	TIM_Cmd(TIM3,ENABLE);//使能或者失能TIMx外设
}


//通用定时器7中断初始化，这里时钟选择为APB1的2倍
//arr：自动重装值 psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz 
//通用定时器中断初始化 
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
	
	TIM_Cmd(TIM7,ENABLE);//开启定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}

//定时器7中断服务程序		    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART1_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIM7更新中断标志    
		TIM_Cmd(TIM7, DISABLE);  //关闭TIM7 
	}	    
}
 






