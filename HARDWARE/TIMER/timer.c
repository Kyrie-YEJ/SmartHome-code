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
extern int position;//OLED��>��������
extern float mq135_ppm;
extern float Light;
extern u8 Temp_Max,Humi_Max,Temp_Min,Humi_Min;//�¶���ֵ,ʪ����ֵ,
extern float Mq135_Max;//����������ֵ
extern u8 Windows_Status;//�����򿪱�־
extern u8 lastmove;//��һ�δ����������Ĭ�ϳ�ʼ״̬Ϊ�� 1���� 0����
extern u8 voice_command;//����ʶ��ָ��
extern u8 HomeMode;//�Զ�ģʽ��־λ 0���ֶ�ģʽ 1���Զ�ģʽ 2:���ģʽ 3��˯��ģʽ

//���±�־λ���ڷ��Ͷ��ŵľ�����־
extern u8 Temp_message;
extern u8 Temp_lastflag;//��һ���¶ȵı�־��������ֵΪ0��������ֵΪ1
extern u8 Temp_nowflag;//��ǰ�¶ȵı�־
extern u8 Humi_message;
extern u8 Humi_lastflag;//��һ��ʪ�ȵı�־��������ֵΪ0��������ֵΪ1
extern u8 Humi_nowflag;//��ǰʪ�ȵı�־
extern u8 MQ135_message;
extern u8 MQ135_lastflag;//��һ�ο��������ı�־��������ֵΪ0��������ֵΪ1
extern u8 MQ135_nowflag;//��ǰ���������ı�־
extern u8 mode;
extern u8 changeflag;
extern int Lcd_Page;
extern vu16 USART1_RX_STA;
//��ʱ��3��ʼ������
void Timer3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	// NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_Cmd(TIM3,ENABLE);
}

//��ʱ��5��ʼ������
void Timer5_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	// NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_Cmd(TIM5,DISABLE);
}

/********************************��ʱ��3�жϴ�����**************************************/
//�������ܣ���ȡ���������ݣ������ݹ�ǿ���Ƶ���Ŀ���(ģ�ⴰ���Ŀ���)
u8 TIM3_count1=0;
u16 TIM3_count2=0;
extern u8 OK;//����������ѭ����־λ
extern u8 AlarmClock[3][3];//���ӣ������涨��������3�����Ӿ�ȷ����,��3λΪѡ�����ӱ�־λ;��0��ʱ����1���֣���2��ѡ�б�־
extern u8 leaveflag;//��ұ�־λ 0���ڼ� 1�����
extern u8 Led_Status;//LED�Ʊ�־λ
extern u8 Door_Status;//�Ŵ򿪱�־
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
			DHT11_Read_Data(&temperatureH,&temperatureL,&humidityH,&humidityL);	//��ȡ��ʪ��ֵ	
			Light=Lsens_Get_Val();//��ȡ����ǿ��
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
					//DEBUG_LOG("����1��ѡ��");
					if(AlarmClock[0][0]==calendar.hour && AlarmClock[0][1]==calendar.min && calendar.sec<15)
					{
						//DEBUG_LOG("����1����");
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
					//DEBUG_LOG("����2��ѡ��");
					if(AlarmClock[1][0]==calendar.hour && AlarmClock[1][1]==calendar.min && calendar.sec<15)
					{
						//DEBUG_LOG("����2����");
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
					//DEBUG_LOG("����3��ѡ��");
					if(AlarmClock[2][0]==calendar.hour && AlarmClock[2][1]==calendar.min && calendar.sec<15)
					{
						//DEBUG_LOG("����3����");
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
//		if(HomeMode==0)//�ֶ�ģʽ
//		{
//			if(Windows_Status==1)
//			{
//				if(lastmove==0)//�����һ�δ���״̬Ϊ�أ��򿪴�
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=1;
//					//voice_command=0;
//				}
//			}
//			else if(Windows_Status==0)
//			{
//				if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=0;
//					//voice_command=0;
//				}
//			}
//		}
//		else if(HomeMode==1)//�Զ�ģʽ
//		{
//			if(Lsens_Get_Val()>80)//�����ǿ>70 ����
//			{
//				if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=0;
//					//voice_command=0;
//				}
//			}
//			else if(Lsens_Get_Val()<20 || mq135_ppm>500)//�����ǿ<30  ����
//			{
//				if(lastmove==0)//�����һ�δ���״̬Ϊ�أ��򿪴�
//				{
//					TIM_Cmd(TIM2, ENABLE);
//					Windows_Status=1;
//					//voice_command=0;
//				}
//			}	
//			//if(temperatureH>Temp_Max || humidityH>Humi_Max || mq135_ppm>Mq135_Max)//��ֵ����mq135_ppm�������⣬��ʱ����Ҫ
//			if(temperatureH>Temp_Max || humidityH>Humi_Max)//��ֵ����
//			{
//				LED1=!LED1;
//				BEEP=!BEEP;
//				if(mq135_ppm>Mq135_Max)
//				{
//					if(lastmove==0)//�����һ�δ���״̬Ϊ�أ��򿪴�
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
//			TIM_SetCompare2(TIM3,Lsens_Get_Val()*2);//PC7�˿ڿ��Ƶ�LED������������ı仯������PWM����
//		}
//		else if(HomeMode==2)//���ģʽ
//		{
//			if(leaveflag==1)
//			{
//				BEEP=!BEEP;//�������˴���
//				LED1=!LED1;
//			}
//			else
//			{
//				BEEP=0;
//				LED1=1;
//			}
//			if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
//			{
//				TIM_Cmd(TIM2, ENABLE);
//				Windows_Status=0;
//				//voice_command=0;
//			}
//			Led_Status=1;//�ص�
//			LED0=1;
//			Door_Status=0;//����
//			TIM_SetCompare1(TIM3, 185);
//		}
//		else if(HomeMode==3)//��Ӱģʽ
//		{
//			if(lastmove==1)//�����һ�δ���״̬Ϊ������ش�
//			{
//				TIM_Cmd(TIM2, ENABLE);
//				Windows_Status=0;
//				//voice_command=0;
//			}
//			Led_Status=1;//�ص�
//			LED0=1;
//		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//����ն˱�־λ��Ϊ�´��ж���׼��
	}
}

//u8 count5=0;//����ת̫���˼���
//void TIM5_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM5,TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);//����ն˱�־λ��Ϊ�´��ж���׼��
////		if(tp_dev.sta&TP_PRES_DOWN)			//������������
////		{
////			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
////			if(changeflag==1)//�������ҳ���л����
////			{
////				changeflag=0;
////				LCD_Clear(WHITE);
////			}
////		}
//    //LCD_ShowPage(Lcd_Page);	
//		//LCD_ShowPage(6);	
//		//OLED_Show_mode(mode);
//		LCD_Fill(120,80,240,208,WHITE);
//		LCD_ShowPicture(120,80,114,128,skyman[count5]);//��֡��ʾ̫����
//		count5++;
//		if(count5>47)
//			count5=0;
//	}
//}
/*************************************************************************************/
//LED�����ƣ�PC7
//�����PC6
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;   //����һ���ṹ�������������ʼ��GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//����һ���ṹ�������������ʼ����ʱ��

	TIM_OCInitTypeDef TIM_OCInitStructure;//����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	/* ����ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	//�����������
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;// PC6
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	//LED��������
	//���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOC.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIO
	
	//TIM3��ʱ����ʼ��
	TIM_TimeBaseInitStructure.TIM_Period = arr; //PWM Ƶ��=72000/(199+1)=36Khz//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//�ı�ָ���ܽŵ�ӳ��	//pC6

	//PWM��ʼ��	  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;

	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	//ע��˴���ʼ��ʱTIM_OC1Init������TIM_OCInit������������Ϊ�̼���İ汾��һ����
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܻ���ʧ��TIMx����
}


//ͨ�ö�ʱ��7�жϳ�ʼ��������ʱ��ѡ��ΪAPB1��2��
//arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz 
//ͨ�ö�ʱ���жϳ�ʼ�� 
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
	
	TIM_Cmd(TIM7,ENABLE);//������ʱ��7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}

//��ʱ��7�жϷ������		    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART1_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM7, DISABLE);  //�ر�TIM7 
	}	    
}
 






