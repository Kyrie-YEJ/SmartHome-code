#include "led.h"
#include "delay.h"
#include "key.h"
#include "beep.h"
#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "timer.h"
#include "oled.h"
#include "lcd.h"
#include "touch.h"
#include "dht11.h" 	 
#include "adc.h"
#include "motor.h"
#include "exit.h"
#include "sr301.h"
#include "mq135.h"
#include "lsens.h"
#include "rtc.h"
#include "remote.h"
#include "esp8266.h"
#include "onenet.h"
#include "sim900a.h"
#include "24cxx.h"

char temp[20],humi[20],ADC_data[20],ADC_vol[20],date[20],clock[20];//date:年/月/日 clock:时/分/秒
extern u8 temperatureH,temperatureL,humidityH,humidityL;
char MQ135_data[20];
char MQ135_vol[20];
extern u8 flag;
u16 adc_data;
float adc_vol;
u16 mq135_data;
float mq135_vol;
extern u8 light;
extern uint8_t len;
extern u8 Buf[1000];
extern uint8_t Res;
extern uint16_t res;
extern u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern u16 USART_RX_STA;       //接收状态标记	 
extern u8 stop_flag;//步进电机停止标志
extern int position;//OLED“>”的坐标
extern u8 voice_command;//语音识别指令
u8 HomeMode=2;//自动模式标志位  0：自动模式 1:离家模式 2：手动模式 3：电影模式
u8 leaveflag=0;//离家标志位 0：在家 1：离家
 
u8 mode=0;
u8 ClearOled=0;//为了解决红外遥控导致的OLED刷屏BUG而设定的标志位，用在remote.c里
float Mq135_Max=3000;//气体阈值
u8 Temp_Max=35;//温度最大阈值
u8 Humi_Max=80;//湿度最大阈值
u8 Temp_Min=10;//温度最小阈值
u8 Humi_Min=20;//湿度最小阈值
u8 Light_Max=70;//光度阈值
u8 lastmove=0;//上一次窗户打开情况，默认初始状态为关 1：开 0：关
u8 Led_Status = 0;//LED灯标志位
u8 Door_Status=0;//门打开标志
u8 SR301_Status=0;//是否有人标志
u8 Beep_Status=0;//警报响标志
u8 Windows_Status=0;//窗户打开标志
u8 Fan_Status=0;//风扇打开标志
u8 face_recognize=0;//人脸识别输入口标志
u8 OK=0;//主函数进入循环标志位
float mq135_ppm;//气体质量值
float Light = 0; //光照度
extern u16 MotorStepCount;//电机转动的拍数
extern int Lcd_Page;//LCD功能页面

char oledBuf[20];
char PUB_BUF[256];//上传数据的buf
const char *devSubTopic[] = {"/mysmarthome/sub"};
const char devPubTopic[] = "/mysmarthome/pub";
u8 ESP8266_INIT_OK = 0;//esp8266初始化完成标志

//以下标志位用于发送短信的警报标志
u8 Temp_message=0;
u8 Temp_lastflag=0;//上一次温度的标志，低于阈值为0，大于阈值为1
u8 Temp_nowflag=0;//当前温度的标志
u8 Humi_message=0;
u8 Humi_lastflag=0;//上一次湿度的标志，低于阈值为0，大于阈值为1
u8 Humi_nowflag=0;//当前湿度的标志
u8 MQ135_message=0;
u8 MQ135_lastflag=0;//上一次空气质量的标志，低于阈值为0，大于阈值为1
u8 MQ135_nowflag=0;//当前空气质量的标志

/*SIM900A短信内容和号码*/
u8 phonecall=0;//打电话标志
char English_message[]={"Hello!!"};                             //英文短信内容
char phonenumber[]={"18813530375"};                             //接受短信的号码
char Chinese_message[]={"4F60597D"};                            //中文短信  内容对应于  你好
char Unicode_phonenumber[]={"00310038003800310033003500330030003300370035"};//接受短信的号码转Unicode码
char Temp_High_Alarm[]={"8B6662A5FF016E295EA68FC79AD8FF0C8BF76CE8610F964D6E29FF01"};//温度过高报警短信
char Temp_Low_Alarm[]={"8B6662A5FF016E295EA68FC74F4EFF0C8BF76CE8610F4FDD6696FF01"};//温度过低，请注意保暖
char Humi_High_Alarm[]={"8B6662A5FF016E7F5EA68FC79AD8FF0C8BF76CE8610F96646E7FFF01"};//湿度过高警报短信
char Humi_Low_Alarm[]={"8B6662A5FF016E7F5EA68FC74F4EFF0C8BF76CE8610F88656C34FF01"};//湿度过低警报短信
char Air_Alarm[]={"8B6662A5FF017A7A6C148D2891CF8FC74F4EFF0C8BF76CE8610F963262A4FF01"};//有害空气警报短信
u8 moodflag=0;//播放mood标志

//JQ8900(MP3)控制指令
unsigned char Start[]={0xAA, 0x02, 0x00, 0xAC};//播放
unsigned char Stop[]={0xAA, 0x03, 0x00, 0xAD};//暂停
unsigned char Voice_Up[]={0xAA, 0x14, 0x00, 0xBE};//音量加
unsigned char Voice_Down[]={0xAA, 0x15, 0x00, 0xBF};//音量减
unsigned char Last_File2[]={0xAA, 0x05, 0x00, 0xAF};//上一首
unsigned char Next_File2[]={0xAA, 0x06, 0x00, 0xB0};//下一首
unsigned char End_Music[]={0xAA, 0x10, 0x00, 0xBA};//结束播放
unsigned char Mood[]={0xAA, 0x07, 0x02, 0x00, 0x0F, 0xC2};//播放mood这首个歌
u8 Music_Voice=25;//音乐音量
u8 Play_flag=0;//0:暂停 1：播放
u8 changeflag=0;//页面切换清屏标志位
u8 Clock_Num=1;//闹钟数量，目前设置最多可设置3个闹钟
u8 AlarmClock[3][3]={0};//闹钟，初步规定最多可设置3个闹钟精确到分,第3位为选中闹钟标志位;第0：时，第1：分，第2：选中标志
u8 clockclear[4]={0};//设置闹钟被选中/取消/删除后的清除特定区域的标志位，1代表点击，0代表每点击；
//clockclear[4] 第1~3位为点击了闹钟选中按钮的标志位，第4位为点击了删除的标志位
extern u8 musicclear[2];//音乐控制界面清除标志位;第1位：标志音量条的清除标志 第2位：标志播放/暂停清除标志
//u8 year,month,date;
extern unsigned char esp32_buf[500];//存放天气信息的数组
u8 weatherflag=0;//获取天气标志 1：正在获取天气 0：没有获取天气
extern Results results[];
char last_nowweather[20];//存储上一次的最新天气情况
char last_dailyweather[3][20];//存储上一次的天气预测情况
extern u8 wififlag1;//wifiesp8266连接标志
extern u8 wififlag2;//wifiesp32连接标志
/************************************************
 ALIENTEK精英STM32开发板实验4
 串口 实验   
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
uint8_t num=0;
int main(void)
{
	u8 t1;
	//int key_val=0;
	u16 t=0;
	u8 cow;
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	EXTIX_Init();
		//DEBUG_LOG("EXTI初始化			[OK]");
	usart1_init(115200);	 //串口初始化为115200
	usart2_init(115200);	 //串口初始化为115200
	usart3_init(9600);	 //串口初始化为9600
	uart4_init(115200);	 //串口初始化为9600
	uart5_init(115200);//与SIM900A通信
//	EXTIX_Init();
	//DEBUG_LOG("\r\n");
		DEBUG_LOG("USART1/2/3/4初始化			[OK]");
 	LED_Init();			     //LED端口初始化
		DEBUG_LOG("LED初始化			[OK]");
	KEY_Init();          //初始化与按键连接的硬件接口
		DEBUG_LOG("KEY初始化			[OK]");
//	EXTIX_Init();
//		DEBUG_LOG("EXTI初始化			[OK]");
	RTC_Init();	  			//RTC初始化
		DEBUG_LOG("RTC初始化			[OK]");
//		EXTIX_Init();
	BEEP_Init();
		DEBUG_LOG("BEEP初始化			[OK]");
	
//	EXTIX_Init();
//		DEBUG_LOG("EXTI初始化			[OK]");
	EXTIX_SR301_Init();
		DEBUG_LOG("SR301初始化			[OK]");
//  LCD_Init();//LCD触摸屏
//		DEBUG_LOG("LCD初始化			[OK]");
//	tp_dev.init();
//		DEBUG_LOG("TP触摸屏初始化			[OK]");
	Adc_Init();		  		 //ADC初始化
		DEBUG_LOG("ADC初始化			[OK]");
	MQ135_Init();
		DEBUG_LOG("MQ135初始化			[OK]");
	Lsens_Init();
		DEBUG_LOG("Lsens初始化			[OK]");
	MOTOR_Init();
		DEBUG_LOG("MOTOR初始化			[OK]");
	EXTIX_Init();
	OLED_Init();
		DEBUG_LOG("OLED初始化			[OK]");
	LCD_Init();//LCD触摸屏
		DEBUG_LOG("LCD初始化			[OK]");
	tp_dev.init();
		DEBUG_LOG("TP触摸屏初始化			[OK]");
	DHT11_Init();
	while(DHT11_Check())	//DHT11初始化	
	{
		//LCD_ShowStr(7,0,"DHT11 Error");LCD1602，已删
		LCD_ShowString(54,96,132,24,24,"DHT11 Error");//星期
		delay_ms(200);
	}
		DEBUG_LOG("DHT11初始化			[OK]");
	LCD_Clear(WHITE);
  LCD_ShowString(54,96,144,24,24,"Waiting For");//星期
  LCD_ShowString(12,96,216,24,24,"WiFi Connection...");//星期
	//OLED_ShowString(0,0,(u8*)">",16,1);
	/*********************************************************/
	if(!ESP8266_INIT_OK){
		OLED_Clear();
		sprintf(oledBuf,"Waiting For");
		OLED_ShowString(16,0,(u8*)oledBuf,16,1);//8*16 “ABC”
		sprintf(oledBuf,"WiFi");
		OLED_ShowString(48,18,(u8*)oledBuf,16,1);//8*16 “ABC”
		sprintf(oledBuf,"Connection");
		OLED_ShowString(24,36,(u8*)oledBuf,16,1);//8*16 “ABC”
		OLED_Refresh();
	}
	esp32_start_trans(); 								//esp8266进行初始化，连接wifi 
	ESP8266_Init();					//初始化ESP8266	
	
	OLED_Clear();
	sprintf(oledBuf,"Waiting For");
	OLED_ShowString(16,0,(u8*)oledBuf,16,1);//8*16 “ABC”
	sprintf(oledBuf,"MQTT Server");
	OLED_ShowString(16,18,(u8*)oledBuf,16,1);//8*16 “ABC”
	sprintf(oledBuf,"Connection");
	OLED_ShowString(24,36,(u8*)oledBuf,16,1);//8*16 “ABC”
	OLED_Refresh();	
	while(OneNet_DevLink()){//接入OneNET
		delay_ms(500);
	}		
	
//	OLED_Clear();	
//	LCD_Clear(WHITE);
	OneNet_Subscribe(devSubTopic, 1);
	//系统首次上电，初始化数据
	if(AT24CXX_ReadOneByte(80) != 0xA0)
	{
		u8 i;
		//向EEPROM中写入需保存的数据
		AT24CXX_WriteOneByte(60,Temp_Max);
		AT24CXX_WriteOneByte(61,Humi_Max);
		AT24CXX_WriteOneByte(62,Light_Max);
		AT24CXX_WriteLenByte(63,Mq135_Max,4);
		AT24CXX_WriteOneByte(67,Music_Voice);
		AT24CXX_WriteOneByte(68,Clock_Num);
		for(i=0;i<3;i++)
		{
			AT24CXX_Write(69,AlarmClock[i],3);
		}
		AT24CXX_WriteOneByte(80,0xA0);
	}
	//读出保存的数据，实现掉电保存功能
	Temp_Max=AT24CXX_ReadOneByte(60);
	Humi_Max=AT24CXX_ReadOneByte(61);
	Light_Max=AT24CXX_ReadOneByte(62);
	Mq135_Max=(float)AT24CXX_ReadLenByte(63,4);
	Music_Voice=AT24CXX_ReadOneByte(67);
	Clock_Num=AT24CXX_ReadOneByte(68);
	for(cow=0;cow<3;cow++)
	{
		AT24CXX_Read(69+3*cow,AlarmClock[cow],3);
	}
	//定时器初始化放在这是为了不中断esp8266与云服务器的稳定连
	Timer2_Init(19,7199);//定时器初始化TIM_Cmd(TIM2,DISABLE)，步进电机运行，中断优先级高，不会被打断
	Timer3_Init(399,7199);
	Timer5_Init(499,7199);//给天气时钟的旋转太空人用，初始化时暂时DISABLE，等切换到天气页面时再使能
	TIM3_PWM_Init(199,7199);
	OLED_Clear();	
	LCD_Clear(WHITE);//放在这里是解决LCD每次重启后显示的第一个页面会有太空人残影
//	EXTIX_Init();
//		DEBUG_LOG("EXTI初始化			[OK]");
//	DEBUG_LOG("系统初始化成功\r\n");
	/*********************************************************/	
	get_current_weather(); 								//获取天气
	delay_ms(1000);
	get_threedays_weather();
	strcpy(last_nowweather,results[0].now.text);
	for(t1=0;t1<3;t1++)
	{
		strcpy(last_dailyweather[t1],results[0].daily[t1].text_day);
	}
	while(1)
	{	
		t++;
		OK=1;
		Led_Status=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
		Beep_Status=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8);
		Fan_Status=GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_5);
		face_recognize=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
		if(face_recognize==1)//如果人脸识别成功，则开门
		{
			TIM_SetCompare1(TIM3, 195);
			delay_ms(100);
			Door_Status=1;
		}
		if(PEN==0)//如果触摸屏被按下
		{
			if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))
			{
				tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
				tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff; 
			}
			//DEBUG_LOG("中断成功，触摸坐标为：x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
			while(PEN==0);
			if(Lcd_Page>=0 && Lcd_Page<=6)//只有在切换功能页面的时候可以起作用（目前一共有6个模块）
			{
				changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//如果按下左键
				{
					//LCD_Clear(WHITE);
					Lcd_Page--;//LCD功能页面
					if(Lcd_Page<0)
						Lcd_Page=6;
				}
				else if(tp_dev.x[0]>192&&tp_dev.x[0]<240 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//如果按下右键
				{
					//LCD_Clear(WHITE);
					Lcd_Page++;//LCD功能页面
					if(Lcd_Page>6)
						Lcd_Page=0;
				}
				else if(tp_dev.x[0]>56&&tp_dev.x[0]<184 && tp_dev.y[0]>96&&tp_dev.y[0]<224)//如果选中对应功能页面
				{
					if(Lcd_Page==0)
					{
						//LCD_Clear(WHITE);清屏函数放在中断不起作用
						Lcd_Page=7;
					}
					else if(Lcd_Page==1)
					{
						//LCD_Clear(LIGHTBLUE);
						Lcd_Page=8;
					}
					else if(Lcd_Page==2)
					{
						//LCD_Clear(WHITE);
						Lcd_Page=9;
					}
					else if(Lcd_Page==3)
					{
						//LCD_Clear(WHITE);
						Lcd_Page=10;
					}
					else if(Lcd_Page==4)
					{
						//LCD_Clear(WHITE);
						Lcd_Page=11;
					}
					else if(Lcd_Page==5)
					{
						LCD_Clear(WHITE);//因为太空人用的是中断，所以清屏要写在这里
						TIM_Cmd(TIM5,ENABLE);//启动旋转太空人
						Lcd_Page=12;
					}
					else if(Lcd_Page==6)
					{
						Lcd_Page=13;
					}
				}
			}
			if(Lcd_Page==9)//设置页面的阈值加减
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
				//向AT24C02中写入改动后的阈值
				//AT24CXX_WriteOneByte(60,Mq135_Max);
				AT24CXX_WriteOneByte(60,Temp_Max);
				AT24CXX_WriteOneByte(61,Humi_Max);
				AT24CXX_WriteOneByte(62,Light_Max);
				AT24CXX_WriteLenByte(63,Mq135_Max,4);
			}
			else if(Lcd_Page==10)//控制二级界面
			{
				if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>32&&tp_dev.y[0]<64)//点击风扇按钮
				{
					if(Fan_Status==1)
					{
						OUT=0;
						Fan_Status=0;
					}
					else
					{
						Fan_Status=1;
						OUT=1;
					}
				}
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>64&&tp_dev.y[0]<96)//点击开窗按钮
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
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>192&&tp_dev.y[0]<224)//点击获取当前天气
				{
					get_current_weather();
				}		
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>224&&tp_dev.y[0]<256)//点击获取未来3天天气
				{
					get_threedays_weather();
				}	
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回主页面
				{
					//LCD_Clear(WHITE);
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=3;
				}
			}
			else if(Lcd_Page==8)//音乐播放界面
			{
				if(tp_dev.x[0]>24&&tp_dev.x[0]<56 && tp_dev.y[0]>16&&tp_dev.y[0]<48)//减音量
				{
					if(Music_Voice>=2)//此处出现一个BUG,当判断条件为if(Music_Voice>=1)时，当音量减到0时界面会出现绿色闪屏
					{
						musicclear[0]=1;//音量条清色标志位
						//LCD_Fill(58,26,54+Music_Voice*4,38,WHITE);//音量条清色
						Music_Voice--;
						//此行后期需增加通过串口向MP3模块发送指令,可能需要暂停其他中断的使能
						Usart_SendString(USART3,Voice_Down,4);//串口3发送减音量指令
						AT24CXX_WriteOneByte(67,Music_Voice);//保存改动后的音量
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
						Usart_SendString(USART3,Voice_Up,4);//串口3发送加音量指令
						AT24CXX_WriteOneByte(67,Music_Voice);//保存改动后的音量
					}
				}
				else if(tp_dev.y[0]>216&&tp_dev.y[0]<276)//播放/暂停/上下首按键切换
				{
					if(moodflag==0)
					{
						moodflag=1;
						Usart_SendString(USART3,Mood,6);//第一次进音乐界面后首先放mood这首歌
					}
					if(tp_dev.x[0]>90&&tp_dev.x[0]<150)//如果按下的是播放/暂停键
					{
						musicclear[1]=1;//播放/停止按键清色标志位
						if(Play_flag==0)
						{ 
							Play_flag=1;//播放音乐，显示播放图标(在lcd.c里面进行切换)
							Usart_SendString(USART3,Start,4);//串口3发送播放指令
						}
						else
						{
							Play_flag=0;//暂停播放，显示暂停图标(在lcd.c里面进行切换)
							Usart_SendString(USART3,Stop,4);//串口3发送暂停指令
						}
					}
					else if(tp_dev.x[0]>8&&tp_dev.x[0]<56)//如果按下的是上一首切换键
					{
						musicclear[1]=1;//播放/停止按键清色标志位
						Play_flag=1;//播放音乐，显示播放图标(在lcd.c里面进行切换)
						Usart_SendString(USART3,Last_File2,4);//串口3发送暂停指令
					}
					else if(tp_dev.x[0]>184&&tp_dev.x[0]<232)//如果按下的是下一首切换键
					{
						musicclear[1]=1;//播放/停止按键清色标志位
						Play_flag=1;//播放音乐，显示播放图标(在lcd.c里面进行切换)
						Usart_SendString(USART3,Next_File2,4);//串口3发送暂停指令
					}
				}
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回主页面
				{
					//LCD_Clear(WHITE);
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=1;
				}
			}
			else if(Lcd_Page==11)//模式选择操作界面
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
			else if(Lcd_Page==7)//闹钟设置界面
			{
				//点击选中启动闹钟按钮(并且存在待设置的闹钟)加tp_dev.y[0]<272条件是防止和添加时钟符号冲突
				if(tp_dev.x[0]>16 && tp_dev.x[0]<228 && tp_dev.y[0]>16 && tp_dev.y[0]<272 && Clock_Num>0)
				{
					u8 k=0;
					for(k=0;k<Clock_Num;k++)
					{
						if(tp_dev.x[0]<180 && tp_dev.y[0]>16 && tp_dev.y[0]<248)//如果点击的是加减时间区域
						{
							if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//时增
							{
								AlarmClock[k][0]++;
								if(AlarmClock[k][0]>23)
									AlarmClock[k][0]=0;
								AT24CXX_WriteOneByte(69+3*k,AlarmClock[k][0]);
							}
							else if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(66+k*80) && tp_dev.y[0]<(88+k*80))//时减
							{
								if(AlarmClock[k][0]>0)
									AlarmClock[k][0]--;
								else
									AlarmClock[k][0]=23;
								AT24CXX_WriteOneByte(69+3*k,AlarmClock[k][0]);
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//分增
							{
								AlarmClock[k][1]++;
								if(AlarmClock[k][1]>59)
									AlarmClock[k][1]=0;
								AT24CXX_WriteOneByte(70+3*k,AlarmClock[k][1]);
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(66+k*80) &&tp_dev.y[0]<(88+k*80))//分减
							{
								if(AlarmClock[k][1]>0)
									AlarmClock[k][1]--;
								else
									AlarmClock[k][1]=59;
								AT24CXX_WriteOneByte(70+3*k,AlarmClock[k][1]);
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
							AT24CXX_WriteOneByte(71+3*k,AlarmClock[k][2]);
						}
					}
				} 
				else if(tp_dev.y[0]>272&&tp_dev.y[0]<320)//点击添加/删除闹钟按钮
				{
					if(tp_dev.x[0]>120&&tp_dev.x[0]<168)//按下的是删除按钮
					{
						//int j=0;
						if(Clock_Num>0)//防止越界
						Clock_Num--;
						clockclear[3]=1;//给lcd.c标记点击了对应闹钟按钮，lcd.c里面的显示函数进行对应的区域的清除
						//LCD_Fill(0,16+Clock_Num*80,240,88+Clock_Num*80,WHITE);//删除后需要消除的闹钟记录（清除画点数据）
						AlarmClock[Clock_Num][0]=0;
						AlarmClock[Clock_Num][1]=0;
						AlarmClock[Clock_Num][2]=0;//删除闹钟后同样要删除其时间及选中标志位记录
						AT24CXX_WriteOneByte(68,Clock_Num);
					}
					else if(tp_dev.x[0]>184&&tp_dev.x[0]<232)//按下的是添加按钮
					{
						Clock_Num++;
						if(Clock_Num>3)//防止闹钟数越界，初设最多设置3个闹钟
							Clock_Num=3;
						AT24CXX_WriteOneByte(68,Clock_Num);
					}
					else if(tp_dev.x[0]>0&&tp_dev.x[0]<48)//返回进入前的页面，因为放在了闹钟添加/删除同一行，所以代码放在此处同讨论
					{
						//LCD_Clear(WHITE);
						changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
						Lcd_Page=0;
					}
				}
			}
			else if(Lcd_Page==12)//天气时钟显示界面
			{
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回进入前的页面
				{
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					TIM_Cmd(TIM5,DISABLE);
					Lcd_Page=5;
				}
			} 
			else if(Lcd_Page==13)//天气时钟显示界面
			{
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//返回进入前的页面
				{
					changeflag=1;//页面切换清屏标志位,解决在中断函数中LCD_Clear(WHITE)不起作用的BUG
					Lcd_Page=6;
				}
			}
			//LCD_Clear(WHITE);
		}
		if(changeflag==1)
		{
			changeflag=0;
			LCD_Clear(WHITE);
		}
		LCD_ShowPage(Lcd_Page);	
		OLED_Show_mode(mode);
		
		if(phonecall==1)
		{
			phonecall=0;
			sim900a_call_phone(phonenumber);
		}
		/********************************短信警报******************************/
		if(Temp_message==1)//发送温度短信警报
		{
			Temp_message=0;
			sim900a_send_Chinese_message(Temp_High_Alarm,Unicode_phonenumber);
		}
		if(Humi_message==1)//发送湿度短信警报
		{
			Humi_message=0;
			sim900a_send_Chinese_message(Humi_High_Alarm,Unicode_phonenumber);
		}
		if(MQ135_message==1)//发送空气质量短信警报
		{
			MQ135_message=0;
			sim900a_send_Chinese_message(Air_Alarm,Unicode_phonenumber);
		}
		/*********************************************************************/
		
		if(timeCount % 25 == 0)//1000ms / 25 = 40 一秒执行一次
		{
			DEBUG_LOG(" | 湿度：%d.%d %% | 温度：%d.%d ℃ | 光照度：%.1f lx | 指示灯：%s |",humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?"关闭":"【启动】");
		}
		if(++timeCount >= 50)	// 5000ms / 25 = 200 发送间隔5000ms
		{
			UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			sprintf(PUB_BUF,"{\"Humi\":%d.%d,\"Temp\":%d.%d,\"Light\":%.lf,\"Aq\":%.lf,\"Led\":%d,\"Beep\":%d,\"Windows\":%d,\"Door\":%d,\"Fan\":%d}",
			humidityH,humidityL,temperatureH,temperatureL,Light,mq135_ppm,Led_Status?0:1,Beep_Status?1:0,Windows_Status?1:0,Door_Status?1:0,Fan_Status?1:0);
			OneNet_Publish(devPubTopic, PUB_BUF);
			
			timeCount = 0;
			ESP8266_Clear();
		}
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
		
		if(t>1000)
		{
			t=0;
			get_current_weather(); 								//获取天气
//			delay_ms(100);
//			get_threedays_weather();//天气预报通过控制界面的按键控制即可
		}
//		if(t>300)
//		{
//			t=0;
//			get_weather();
//			cJSON_WeatherParse((char*)esp32_buf,results);
//			UsartPrintf(USART1,"+++");
//			DEBUG_LOG("%s\r\n",esp32_buf);
//			delay_ms(100);
//			UsartPrintf(USART1,"AT\r\n");
//			delay_ms(100);
//		}
		
		if(HomeMode==0)//自动模式
		{
			if(Lsens_Get_Val()>80)//如果光强>70 过亮
			{
				if(lastmove==1)//如果上一次窗户状态为开，则关窗
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=0;
					//voice_command=0;
				}
			}
			else if(Lsens_Get_Val()<20 || mq135_ppm>500)//如果光强<30  过暗
			{
				if(lastmove==0)//如果上一次窗户状态为关，则开窗
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=1;
					//voice_command=0;
				}
			}	
			//if(temperatureH>Temp_Max || humidityH>Humi_Max || mq135_ppm>Mq135_Max)//阈值报警mq135_ppm存在问题，暂时不需要
			if(temperatureH>Temp_Max || humidityH>Humi_Max)//阈值报警
			{
				LED1=!LED1;
				BEEP=!BEEP;
				if(mq135_ppm>Mq135_Max)
				{
					if(lastmove==0)//如果上一次窗户状态为关，则开窗
					{
						TIM_Cmd(TIM2, ENABLE);
						Windows_Status=1;
						//voice_command=0;
					}
				}
			}
			else
			{
				LED1=1;
				BEEP=0;
			}
			TIM_SetCompare2(TIM3,Lsens_Get_Val()*2);//PC7端口控制的LED随光敏传感器的变化而进行PWM调光
		}
		
		else if(HomeMode==1)//离家模式
		{
			if(leaveflag==1)
			{
				BEEP=!BEEP;//警报有人闯入
				LED1=!LED1;
			}
			else
			{
				BEEP=0;
				LED1=1;
			}
			if(lastmove==1)//如果上一次窗户状态为开，则关窗
			{
				TIM_Cmd(TIM2, ENABLE);
				Windows_Status=0;
				//voice_command=0;
			}
			Led_Status=1;//关灯
			LED0=1;
			Door_Status=0;//关门
			TIM_SetCompare1(TIM3, 185);
			OUT=0;//关风扇
		}
		else if(HomeMode==2)//手动模式
		{
			if(Windows_Status==1)
			{
				if(lastmove==0)//如果上一次窗户状态为关，则开窗
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=1;
					//voice_command=0;
				}
			}
			else if(Windows_Status==0)
			{
				if(lastmove==1)//如果上一次窗户状态为开，则关窗
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=0;
					//voice_command=0;
				}
			}
		}
		else if(HomeMode==3)//电影模式
		{
			if(lastmove==1)//如果上一次窗户状态为开，则关窗
			{
				TIM_Cmd(TIM2, ENABLE);
				Windows_Status=0;
				//voice_command=0;
			}
			Led_Status=1;//关灯
			LED0=1;
		}
  }
}

