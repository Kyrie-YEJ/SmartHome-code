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

char temp[20],humi[20],ADC_data[20],ADC_vol[20],date[20],clock[20];//date:��/��/�� clock:ʱ/��/��
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
extern u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
extern u16 USART_RX_STA;       //����״̬���	 
extern u8 stop_flag;//�������ֹͣ��־
extern int position;//OLED��>��������
extern u8 voice_command;//����ʶ��ָ��
u8 HomeMode=2;//�Զ�ģʽ��־λ  0���Զ�ģʽ 1:���ģʽ 2���ֶ�ģʽ 3����Ӱģʽ
u8 leaveflag=0;//��ұ�־λ 0���ڼ� 1�����
 
u8 mode=0;
u8 ClearOled=0;//Ϊ�˽������ң�ص��µ�OLEDˢ��BUG���趨�ı�־λ������remote.c��
float Mq135_Max=3000;//������ֵ
u8 Temp_Max=35;//�¶������ֵ
u8 Humi_Max=80;//ʪ�������ֵ
u8 Temp_Min=10;//�¶���С��ֵ
u8 Humi_Min=20;//ʪ����С��ֵ
u8 Light_Max=70;//�����ֵ
u8 lastmove=0;//��һ�δ����������Ĭ�ϳ�ʼ״̬Ϊ�� 1���� 0����
u8 Led_Status = 0;//LED�Ʊ�־λ
u8 Door_Status=0;//�Ŵ򿪱�־
u8 SR301_Status=0;//�Ƿ����˱�־
u8 Beep_Status=0;//�������־
u8 Windows_Status=0;//�����򿪱�־
u8 Fan_Status=0;//���ȴ򿪱�־
u8 face_recognize=0;//����ʶ������ڱ�־
u8 OK=0;//����������ѭ����־λ
float mq135_ppm;//��������ֵ
float Light = 0; //���ն�
extern u16 MotorStepCount;//���ת��������
extern int Lcd_Page;//LCD����ҳ��

char oledBuf[20];
char PUB_BUF[256];//�ϴ����ݵ�buf
const char *devSubTopic[] = {"/mysmarthome/sub"};
const char devPubTopic[] = "/mysmarthome/pub";
u8 ESP8266_INIT_OK = 0;//esp8266��ʼ����ɱ�־

//���±�־λ���ڷ��Ͷ��ŵľ�����־
u8 Temp_message=0;
u8 Temp_lastflag=0;//��һ���¶ȵı�־��������ֵΪ0��������ֵΪ1
u8 Temp_nowflag=0;//��ǰ�¶ȵı�־
u8 Humi_message=0;
u8 Humi_lastflag=0;//��һ��ʪ�ȵı�־��������ֵΪ0��������ֵΪ1
u8 Humi_nowflag=0;//��ǰʪ�ȵı�־
u8 MQ135_message=0;
u8 MQ135_lastflag=0;//��һ�ο��������ı�־��������ֵΪ0��������ֵΪ1
u8 MQ135_nowflag=0;//��ǰ���������ı�־

/*SIM900A�������ݺͺ���*/
u8 phonecall=0;//��绰��־
char English_message[]={"Hello!!"};                             //Ӣ�Ķ�������
char phonenumber[]={"18813530375"};                             //���ܶ��ŵĺ���
char Chinese_message[]={"4F60597D"};                            //���Ķ���  ���ݶ�Ӧ��  ���
char Unicode_phonenumber[]={"00310038003800310033003500330030003300370035"};//���ܶ��ŵĺ���תUnicode��
char Temp_High_Alarm[]={"8B6662A5FF016E295EA68FC79AD8FF0C8BF76CE8610F964D6E29FF01"};//�¶ȹ��߱�������
char Temp_Low_Alarm[]={"8B6662A5FF016E295EA68FC74F4EFF0C8BF76CE8610F4FDD6696FF01"};//�¶ȹ��ͣ���ע�Ᵽů
char Humi_High_Alarm[]={"8B6662A5FF016E7F5EA68FC79AD8FF0C8BF76CE8610F96646E7FFF01"};//ʪ�ȹ��߾�������
char Humi_Low_Alarm[]={"8B6662A5FF016E7F5EA68FC74F4EFF0C8BF76CE8610F88656C34FF01"};//ʪ�ȹ��;�������
char Air_Alarm[]={"8B6662A5FF017A7A6C148D2891CF8FC74F4EFF0C8BF76CE8610F963262A4FF01"};//�к�������������
u8 moodflag=0;//����mood��־

//JQ8900(MP3)����ָ��
unsigned char Start[]={0xAA, 0x02, 0x00, 0xAC};//����
unsigned char Stop[]={0xAA, 0x03, 0x00, 0xAD};//��ͣ
unsigned char Voice_Up[]={0xAA, 0x14, 0x00, 0xBE};//������
unsigned char Voice_Down[]={0xAA, 0x15, 0x00, 0xBF};//������
unsigned char Last_File2[]={0xAA, 0x05, 0x00, 0xAF};//��һ��
unsigned char Next_File2[]={0xAA, 0x06, 0x00, 0xB0};//��һ��
unsigned char End_Music[]={0xAA, 0x10, 0x00, 0xBA};//��������
unsigned char Mood[]={0xAA, 0x07, 0x02, 0x00, 0x0F, 0xC2};//����mood���׸���
u8 Music_Voice=25;//��������
u8 Play_flag=0;//0:��ͣ 1������
u8 changeflag=0;//ҳ���л�������־λ
u8 Clock_Num=1;//����������Ŀǰ������������3������
u8 AlarmClock[3][3]={0};//���ӣ������涨��������3�����Ӿ�ȷ����,��3λΪѡ�����ӱ�־λ;��0��ʱ����1���֣���2��ѡ�б�־
u8 clockclear[4]={0};//�������ӱ�ѡ��/ȡ��/ɾ���������ض�����ı�־λ��1��������0����ÿ�����
//clockclear[4] ��1~3λΪ���������ѡ�а�ť�ı�־λ����4λΪ�����ɾ���ı�־λ
extern u8 musicclear[2];//���ֿ��ƽ��������־λ;��1λ����־�������������־ ��2λ����־����/��ͣ�����־
//u8 year,month,date;
extern unsigned char esp32_buf[500];//���������Ϣ������
u8 weatherflag=0;//��ȡ������־ 1�����ڻ�ȡ���� 0��û�л�ȡ����
extern Results results[];
char last_nowweather[20];//�洢��һ�ε������������
char last_dailyweather[3][20];//�洢��һ�ε�����Ԥ�����
extern u8 wififlag1;//wifiesp8266���ӱ�־
extern u8 wififlag2;//wifiesp32���ӱ�־
/************************************************
 ALIENTEK��ӢSTM32������ʵ��4
 ���� ʵ��   
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
uint8_t num=0;
int main(void)
{
	u8 t1;
	//int key_val=0;
	u16 t=0;
	u8 cow;
	unsigned short timeCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	EXTIX_Init();
		//DEBUG_LOG("EXTI��ʼ��			[OK]");
	usart1_init(115200);	 //���ڳ�ʼ��Ϊ115200
	usart2_init(115200);	 //���ڳ�ʼ��Ϊ115200
	usart3_init(9600);	 //���ڳ�ʼ��Ϊ9600
	uart4_init(115200);	 //���ڳ�ʼ��Ϊ9600
	uart5_init(115200);//��SIM900Aͨ��
//	EXTIX_Init();
	//DEBUG_LOG("\r\n");
		DEBUG_LOG("USART1/2/3/4��ʼ��			[OK]");
 	LED_Init();			     //LED�˿ڳ�ʼ��
		DEBUG_LOG("LED��ʼ��			[OK]");
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
		DEBUG_LOG("KEY��ʼ��			[OK]");
//	EXTIX_Init();
//		DEBUG_LOG("EXTI��ʼ��			[OK]");
	RTC_Init();	  			//RTC��ʼ��
		DEBUG_LOG("RTC��ʼ��			[OK]");
//		EXTIX_Init();
	BEEP_Init();
		DEBUG_LOG("BEEP��ʼ��			[OK]");
	
//	EXTIX_Init();
//		DEBUG_LOG("EXTI��ʼ��			[OK]");
	EXTIX_SR301_Init();
		DEBUG_LOG("SR301��ʼ��			[OK]");
//  LCD_Init();//LCD������
//		DEBUG_LOG("LCD��ʼ��			[OK]");
//	tp_dev.init();
//		DEBUG_LOG("TP��������ʼ��			[OK]");
	Adc_Init();		  		 //ADC��ʼ��
		DEBUG_LOG("ADC��ʼ��			[OK]");
	MQ135_Init();
		DEBUG_LOG("MQ135��ʼ��			[OK]");
	Lsens_Init();
		DEBUG_LOG("Lsens��ʼ��			[OK]");
	MOTOR_Init();
		DEBUG_LOG("MOTOR��ʼ��			[OK]");
	EXTIX_Init();
	OLED_Init();
		DEBUG_LOG("OLED��ʼ��			[OK]");
	LCD_Init();//LCD������
		DEBUG_LOG("LCD��ʼ��			[OK]");
	tp_dev.init();
		DEBUG_LOG("TP��������ʼ��			[OK]");
	DHT11_Init();
	while(DHT11_Check())	//DHT11��ʼ��	
	{
		//LCD_ShowStr(7,0,"DHT11 Error");LCD1602����ɾ
		LCD_ShowString(54,96,132,24,24,"DHT11 Error");//����
		delay_ms(200);
	}
		DEBUG_LOG("DHT11��ʼ��			[OK]");
	LCD_Clear(WHITE);
  LCD_ShowString(54,96,144,24,24,"Waiting For");//����
  LCD_ShowString(12,96,216,24,24,"WiFi Connection...");//����
	//OLED_ShowString(0,0,(u8*)">",16,1);
	/*********************************************************/
	if(!ESP8266_INIT_OK){
		OLED_Clear();
		sprintf(oledBuf,"Waiting For");
		OLED_ShowString(16,0,(u8*)oledBuf,16,1);//8*16 ��ABC��
		sprintf(oledBuf,"WiFi");
		OLED_ShowString(48,18,(u8*)oledBuf,16,1);//8*16 ��ABC��
		sprintf(oledBuf,"Connection");
		OLED_ShowString(24,36,(u8*)oledBuf,16,1);//8*16 ��ABC��
		OLED_Refresh();
	}
	esp32_start_trans(); 								//esp8266���г�ʼ��������wifi 
	ESP8266_Init();					//��ʼ��ESP8266	
	
	OLED_Clear();
	sprintf(oledBuf,"Waiting For");
	OLED_ShowString(16,0,(u8*)oledBuf,16,1);//8*16 ��ABC��
	sprintf(oledBuf,"MQTT Server");
	OLED_ShowString(16,18,(u8*)oledBuf,16,1);//8*16 ��ABC��
	sprintf(oledBuf,"Connection");
	OLED_ShowString(24,36,(u8*)oledBuf,16,1);//8*16 ��ABC��
	OLED_Refresh();	
	while(OneNet_DevLink()){//����OneNET
		delay_ms(500);
	}		
	
//	OLED_Clear();	
//	LCD_Clear(WHITE);
	OneNet_Subscribe(devSubTopic, 1);
	//ϵͳ�״��ϵ磬��ʼ������
	if(AT24CXX_ReadOneByte(80) != 0xA0)
	{
		u8 i;
		//��EEPROM��д���豣�������
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
	//������������ݣ�ʵ�ֵ��籣�湦��
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
	//��ʱ����ʼ����������Ϊ�˲��ж�esp8266���Ʒ��������ȶ���
	Timer2_Init(19,7199);//��ʱ����ʼ��TIM_Cmd(TIM2,DISABLE)������������У��ж����ȼ��ߣ����ᱻ���
	Timer3_Init(399,7199);
	Timer5_Init(499,7199);//������ʱ�ӵ���ת̫�����ã���ʼ��ʱ��ʱDISABLE�����л�������ҳ��ʱ��ʹ��
	TIM3_PWM_Init(199,7199);
	OLED_Clear();	
	LCD_Clear(WHITE);//���������ǽ��LCDÿ����������ʾ�ĵ�һ��ҳ�����̫���˲�Ӱ
//	EXTIX_Init();
//		DEBUG_LOG("EXTI��ʼ��			[OK]");
//	DEBUG_LOG("ϵͳ��ʼ���ɹ�\r\n");
	/*********************************************************/	
	get_current_weather(); 								//��ȡ����
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
		if(face_recognize==1)//�������ʶ��ɹ�������
		{
			TIM_SetCompare1(TIM3, 195);
			delay_ms(100);
			Door_Status=1;
		}
		if(PEN==0)//���������������
		{
			if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))
			{
				tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
				tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff; 
			}
			//DEBUG_LOG("�жϳɹ�����������Ϊ��x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
			while(PEN==0);
			if(Lcd_Page>=0 && Lcd_Page<=6)//ֻ�����л�����ҳ���ʱ����������ã�Ŀǰһ����6��ģ�飩
			{
				changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//����������
				{
					//LCD_Clear(WHITE);
					Lcd_Page--;//LCD����ҳ��
					if(Lcd_Page<0)
						Lcd_Page=6;
				}
				else if(tp_dev.x[0]>192&&tp_dev.x[0]<240 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//��������Ҽ�
				{
					//LCD_Clear(WHITE);
					Lcd_Page++;//LCD����ҳ��
					if(Lcd_Page>6)
						Lcd_Page=0;
				}
				else if(tp_dev.x[0]>56&&tp_dev.x[0]<184 && tp_dev.y[0]>96&&tp_dev.y[0]<224)//���ѡ�ж�Ӧ����ҳ��
				{
					if(Lcd_Page==0)
					{
						//LCD_Clear(WHITE);�������������жϲ�������
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
						LCD_Clear(WHITE);//��Ϊ̫�����õ����жϣ���������Ҫд������
						TIM_Cmd(TIM5,ENABLE);//������ת̫����
						Lcd_Page=12;
					}
					else if(Lcd_Page==6)
					{
						Lcd_Page=13;
					}
				}
			}
			if(Lcd_Page==9)//����ҳ�����ֵ�Ӽ�
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
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					Lcd_Page=2;
				}
				//��AT24C02��д��Ķ������ֵ
				//AT24CXX_WriteOneByte(60,Mq135_Max);
				AT24CXX_WriteOneByte(60,Temp_Max);
				AT24CXX_WriteOneByte(61,Humi_Max);
				AT24CXX_WriteOneByte(62,Light_Max);
				AT24CXX_WriteLenByte(63,Mq135_Max,4);
			}
			else if(Lcd_Page==10)//���ƶ�������
			{
				if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>32&&tp_dev.y[0]<64)//������Ȱ�ť
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
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>64&&tp_dev.y[0]<96)//���������ť
				{
					if(Windows_Status==0)
						Windows_Status=1;
					else
						Windows_Status=0;
				}
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>96&&tp_dev.y[0]<128)//������Ű�ť
				{
					if(Door_Status==0)
					{
						Door_Status=1;
						TIM_SetCompare1(TIM3, 195);//����
					}
					else
					{
						Door_Status=0;
						TIM_SetCompare1(TIM3, 185);//����
					}
				}
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>128&&tp_dev.y[0]<160)//���LED��ť
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
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>160&&tp_dev.y[0]<192)//�����������ť
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
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>192&&tp_dev.y[0]<224)//�����ȡ��ǰ����
				{
					get_current_weather();
				}		
				else if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>224&&tp_dev.y[0]<256)//�����ȡδ��3������
				{
					get_threedays_weather();
				}	
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//������ҳ��
				{
					//LCD_Clear(WHITE);
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					Lcd_Page=3;
				}
			}
			else if(Lcd_Page==8)//���ֲ��Ž���
			{
				if(tp_dev.x[0]>24&&tp_dev.x[0]<56 && tp_dev.y[0]>16&&tp_dev.y[0]<48)//������
				{
					if(Music_Voice>=2)//�˴�����һ��BUG,���ж�����Ϊif(Music_Voice>=1)ʱ������������0ʱ����������ɫ����
					{
						musicclear[0]=1;//��������ɫ��־λ
						//LCD_Fill(58,26,54+Music_Voice*4,38,WHITE);//��������ɫ
						Music_Voice--;
						//���к���������ͨ��������MP3ģ�鷢��ָ��,������Ҫ��ͣ�����жϵ�ʹ��
						Usart_SendString(USART3,Voice_Down,4);//����3���ͼ�����ָ��
						AT24CXX_WriteOneByte(67,Music_Voice);//����Ķ��������
					}
				}
				else if(tp_dev.x[0]>176&&tp_dev.x[0]<208 && tp_dev.y[0]>16&&tp_dev.y[0]<48)//������
				{
					if(Music_Voice<=29)
					{
						musicclear[0]=1;//��������ɫ��־λ
						//LCD_Fill(58,26,54+Music_Voice*4,38,WHITE);//��������ɫ
						Music_Voice++;
						//���к���������ͨ��������MP3ģ�鷢��ָ��
						Usart_SendString(USART3,Voice_Up,4);//����3���ͼ�����ָ��
						AT24CXX_WriteOneByte(67,Music_Voice);//����Ķ��������
					}
				}
				else if(tp_dev.y[0]>216&&tp_dev.y[0]<276)//����/��ͣ/�����װ����л�
				{
					if(moodflag==0)
					{
						moodflag=1;
						Usart_SendString(USART3,Mood,6);//��һ�ν����ֽ�������ȷ�mood���׸�
					}
					if(tp_dev.x[0]>90&&tp_dev.x[0]<150)//������µ��ǲ���/��ͣ��
					{
						musicclear[1]=1;//����/ֹͣ������ɫ��־λ
						if(Play_flag==0)
						{ 
							Play_flag=1;//�������֣���ʾ����ͼ��(��lcd.c��������л�)
							Usart_SendString(USART3,Start,4);//����3���Ͳ���ָ��
						}
						else
						{
							Play_flag=0;//��ͣ���ţ���ʾ��ͣͼ��(��lcd.c��������л�)
							Usart_SendString(USART3,Stop,4);//����3������ָͣ��
						}
					}
					else if(tp_dev.x[0]>8&&tp_dev.x[0]<56)//������µ�����һ���л���
					{
						musicclear[1]=1;//����/ֹͣ������ɫ��־λ
						Play_flag=1;//�������֣���ʾ����ͼ��(��lcd.c��������л�)
						Usart_SendString(USART3,Last_File2,4);//����3������ָͣ��
					}
					else if(tp_dev.x[0]>184&&tp_dev.x[0]<232)//������µ�����һ���л���
					{
						musicclear[1]=1;//����/ֹͣ������ɫ��־λ
						Play_flag=1;//�������֣���ʾ����ͼ��(��lcd.c��������л�)
						Usart_SendString(USART3,Next_File2,4);//����3������ָͣ��
					}
				}
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//������ҳ��
				{
					//LCD_Clear(WHITE);
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					Lcd_Page=1;
				}
			}
			else if(Lcd_Page==11)//ģʽѡ���������
			{
				if(tp_dev.x[0]>8&&tp_dev.x[0]<112 && tp_dev.y[0]>92&&tp_dev.y[0]<152)//mode0:�ֶ�ģʽ
					HomeMode=0;
				else if(tp_dev.x[0]>128&&tp_dev.x[0]<232 && tp_dev.y[0]>92&&tp_dev.y[0]<152)//mode1:�Զ�ģʽ
					HomeMode=1;
				else if(tp_dev.x[0]>8&&tp_dev.x[0]<112 && tp_dev.y[0]>168&&tp_dev.y[0]<228)//mode2:���ģʽ
					HomeMode=2;
				else if(tp_dev.x[0]>128&&tp_dev.x[0]<232 && tp_dev.y[0]>168&&tp_dev.y[0]<228)//mode3:��Ӱģʽ
					HomeMode=3;
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//������ҳ��
				{
					//LCD_Clear(WHITE);
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					Lcd_Page=4;
				}
			}
			else if(Lcd_Page==7)//�������ý���
			{
				//���ѡ���������Ӱ�ť(���Ҵ��ڴ����õ�����)��tp_dev.y[0]<272�����Ƿ�ֹ�����ʱ�ӷ��ų�ͻ
				if(tp_dev.x[0]>16 && tp_dev.x[0]<228 && tp_dev.y[0]>16 && tp_dev.y[0]<272 && Clock_Num>0)
				{
					u8 k=0;
					for(k=0;k<Clock_Num;k++)
					{
						if(tp_dev.x[0]<180 && tp_dev.y[0]>16 && tp_dev.y[0]<248)//���������ǼӼ�ʱ������
						{
							if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//ʱ��
							{
								AlarmClock[k][0]++;
								if(AlarmClock[k][0]>23)
									AlarmClock[k][0]=0;
								AT24CXX_WriteOneByte(69+3*k,AlarmClock[k][0]);
							}
							else if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(66+k*80) && tp_dev.y[0]<(88+k*80))//ʱ��
							{
								if(AlarmClock[k][0]>0)
									AlarmClock[k][0]--;
								else
									AlarmClock[k][0]=23;
								AT24CXX_WriteOneByte(69+3*k,AlarmClock[k][0]);
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//����
							{
								AlarmClock[k][1]++;
								if(AlarmClock[k][1]>59)
									AlarmClock[k][1]=0;
								AT24CXX_WriteOneByte(70+3*k,AlarmClock[k][1]);
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(66+k*80) &&tp_dev.y[0]<(88+k*80))//�ּ�
							{
								if(AlarmClock[k][1]>0)
									AlarmClock[k][1]--;
								else
									AlarmClock[k][1]=59;
								AT24CXX_WriteOneByte(70+3*k,AlarmClock[k][1]);
							}
						}
						else if(tp_dev.x[0]>180 && tp_dev.y[0]>(28+k*80) && tp_dev.y[0]<(76+k*80))//�������������k+1(k+1Ϊ����1~3),����������ѡ����ť����
						{
							clockclear[k]=1;//��lcd.c��ǵ���˶�Ӧ���Ӱ�ť��lcd.c�������ʾ�������ж�Ӧ����������
							if(AlarmClock[k][2]==0)
							{
								//LCD_Fill(180,28+k*80,228,76+k*80,WHITE);//��ǰһ�εİ�ť״̬���
								AlarmClock[k][2]=1;
							}
							else
							{
								//LCD_Fill(180,28+k*80,228,76+k*80,WHITE);//��ǰһ�εİ�ť״̬���
								AlarmClock[k][2]=0;
							}
							AT24CXX_WriteOneByte(71+3*k,AlarmClock[k][2]);
						}
					}
				} 
				else if(tp_dev.y[0]>272&&tp_dev.y[0]<320)//������/ɾ�����Ӱ�ť
				{
					if(tp_dev.x[0]>120&&tp_dev.x[0]<168)//���µ���ɾ����ť
					{
						//int j=0;
						if(Clock_Num>0)//��ֹԽ��
						Clock_Num--;
						clockclear[3]=1;//��lcd.c��ǵ���˶�Ӧ���Ӱ�ť��lcd.c�������ʾ�������ж�Ӧ����������
						//LCD_Fill(0,16+Clock_Num*80,240,88+Clock_Num*80,WHITE);//ɾ������Ҫ���������Ӽ�¼������������ݣ�
						AlarmClock[Clock_Num][0]=0;
						AlarmClock[Clock_Num][1]=0;
						AlarmClock[Clock_Num][2]=0;//ɾ�����Ӻ�ͬ��Ҫɾ����ʱ�估ѡ�б�־λ��¼
						AT24CXX_WriteOneByte(68,Clock_Num);
					}
					else if(tp_dev.x[0]>184&&tp_dev.x[0]<232)//���µ�����Ӱ�ť
					{
						Clock_Num++;
						if(Clock_Num>3)//��ֹ������Խ�磬�����������3������
							Clock_Num=3;
						AT24CXX_WriteOneByte(68,Clock_Num);
					}
					else if(tp_dev.x[0]>0&&tp_dev.x[0]<48)//���ؽ���ǰ��ҳ�棬��Ϊ�������������/ɾ��ͬһ�У����Դ�����ڴ˴�ͬ����
					{
						//LCD_Clear(WHITE);
						changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
						Lcd_Page=0;
					}
				}
			}
			else if(Lcd_Page==12)//����ʱ����ʾ����
			{
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//���ؽ���ǰ��ҳ��
				{
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					TIM_Cmd(TIM5,DISABLE);
					Lcd_Page=5;
				}
			} 
			else if(Lcd_Page==13)//����ʱ����ʾ����
			{
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//���ؽ���ǰ��ҳ��
				{
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
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
		/********************************���ž���******************************/
		if(Temp_message==1)//�����¶ȶ��ž���
		{
			Temp_message=0;
			sim900a_send_Chinese_message(Temp_High_Alarm,Unicode_phonenumber);
		}
		if(Humi_message==1)//����ʪ�ȶ��ž���
		{
			Humi_message=0;
			sim900a_send_Chinese_message(Humi_High_Alarm,Unicode_phonenumber);
		}
		if(MQ135_message==1)//���Ϳ����������ž���
		{
			MQ135_message=0;
			sim900a_send_Chinese_message(Air_Alarm,Unicode_phonenumber);
		}
		/*********************************************************************/
		
		if(timeCount % 25 == 0)//1000ms / 25 = 40 һ��ִ��һ��
		{
			DEBUG_LOG(" | ʪ�ȣ�%d.%d %% | �¶ȣ�%d.%d �� | ���նȣ�%.1f lx | ָʾ�ƣ�%s |",humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?"�ر�":"��������");
		}
		if(++timeCount >= 50)	// 5000ms / 25 = 200 ���ͼ��5000ms
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
			get_current_weather(); 								//��ȡ����
//			delay_ms(100);
//			get_threedays_weather();//����Ԥ��ͨ�����ƽ���İ������Ƽ���
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
		
		if(HomeMode==0)//�Զ�ģʽ
		{
			if(Lsens_Get_Val()>80)//�����ǿ>70 ����
			{
				if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=0;
					//voice_command=0;
				}
			}
			else if(Lsens_Get_Val()<20 || mq135_ppm>500)//�����ǿ<30  ����
			{
				if(lastmove==0)//�����һ�δ���״̬Ϊ�أ��򿪴�
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=1;
					//voice_command=0;
				}
			}	
			//if(temperatureH>Temp_Max || humidityH>Humi_Max || mq135_ppm>Mq135_Max)//��ֵ����mq135_ppm�������⣬��ʱ����Ҫ
			if(temperatureH>Temp_Max || humidityH>Humi_Max)//��ֵ����
			{
				LED1=!LED1;
				BEEP=!BEEP;
				if(mq135_ppm>Mq135_Max)
				{
					if(lastmove==0)//�����һ�δ���״̬Ϊ�أ��򿪴�
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
			TIM_SetCompare2(TIM3,Lsens_Get_Val()*2);//PC7�˿ڿ��Ƶ�LED������������ı仯������PWM����
		}
		
		else if(HomeMode==1)//���ģʽ
		{
			if(leaveflag==1)
			{
				BEEP=!BEEP;//�������˴���
				LED1=!LED1;
			}
			else
			{
				BEEP=0;
				LED1=1;
			}
			if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
			{
				TIM_Cmd(TIM2, ENABLE);
				Windows_Status=0;
				//voice_command=0;
			}
			Led_Status=1;//�ص�
			LED0=1;
			Door_Status=0;//����
			TIM_SetCompare1(TIM3, 185);
			OUT=0;//�ط���
		}
		else if(HomeMode==2)//�ֶ�ģʽ
		{
			if(Windows_Status==1)
			{
				if(lastmove==0)//�����һ�δ���״̬Ϊ�أ��򿪴�
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=1;
					//voice_command=0;
				}
			}
			else if(Windows_Status==0)
			{
				if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
				{
					TIM_Cmd(TIM2, ENABLE);
					Windows_Status=0;
					//voice_command=0;
				}
			}
		}
		else if(HomeMode==3)//��Ӱģʽ
		{
			if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
			{
				TIM_Cmd(TIM2, ENABLE);
				Windows_Status=0;
				//voice_command=0;
			}
			Led_Status=1;//�ص�
			LED0=1;
		}
  }
}

