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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
 
   	EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
		GPIO_InitTypeDef  GPIO_InitStructure;
		
		KEY_Init();
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);	 //ʹ��PB,PF�˿�ʱ��
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

		//������PEN��(PF10)��ʼ��
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // PF10�˿�����
//	 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
//	 	GPIO_Init(GPIOF, &GPIO_InitStructure);//PF10��������
//	 	GPIO_SetBits(GPIOF,GPIO_Pin_10);//����	

//   //GPIOF.10	  �ж����Լ��жϳ�ʼ������ �½��ش���
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_PinSource10);
//  	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
//	
//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1�� 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//�����ȼ�0
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//  	NVIC_Init(&NVIC_InitStructure); 
	  
   //GPIOE.3	  �ж����Լ��жϳ�ʼ������ �½��ش��� //KEY1
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

   //GPIOE.4	  �ж����Լ��жϳ�ʼ������  �½��ش���	//KEY0
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


   //GPIOA.0	  �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
 	  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 

  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//ʹ�ܰ���KEY1���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ���KEY0���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
 
}

u8 flag=0;
extern u8 leaveflag;//��ұ�־λ 0���ڼ� 1�����
extern u8 Led_Status;//LED�Ʊ�־λ
extern u8 Windows_Status;//�����򿪱�־
extern u8 Door_Status;//�Ŵ򿪱�־
extern u8 SR301_Status;//�Ƿ����˱�־
extern u8 Beep_Status;//�������־
extern int Lcd_Page;//LCD����ҳ��
extern float Mq135_Max;//������ֵ
extern u8 Temp_Max;//�¶������ֵ
extern u8 Humi_Max;//ʪ�������ֵ
extern u8 Light_Max;//�����ֵ
extern u8 Music_Voice;//��������
extern u8 Play_flag;//0:��ͣ 1������
extern u8 HomeMode;//0���ֶ�ģʽ 1���Զ�ģʽ 2:���ģʽ 3����Ӱģʽ
extern int Clock_Num;//����������Ŀǰ������������3������
extern int AlarmClock[3][3];//���ӣ������涨��������3�����Ӿ�ȷ����,��3λΪѡ�����ӱ�־λ;��0��ʱ����1���֣���2��ѡ�б�־
extern u8 changeflag;//ҳ���л�������־λ
extern u8 clockclear[4];//�������ӱ�ѡ��/ȡ��/ɾ���������ض�����ı�־λ��1��������0����ÿ�����
//clockclear[4] ��1~3λΪ���������ѡ�а�ť�ı�־λ����4λΪ�����ɾ���ı�־λ
extern u8 musicclear[2];//���ֿ��ƽ��������־λ;��1λ����־�������������־ ��2λ����־����/��ͣ�����־
u8 count=0;

//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//����
	if(WK_UP==1)	 	 //WK_UP����
	{			
		if(leaveflag==1)
			leaveflag=0;//�����ұ�־λ
		if(Windows_Status==0)
			Windows_Status=1;
		else 
			Windows_Status=0;
	}
	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ 
}
 
u8 stop_flag=0;
//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY1==0)	 //����KEY1
	{
		if(Door_Status==1)
		{
			Door_Status=0;
			TIM_SetCompare1(TIM3, 185);//����	 
		}
		else if(Door_Status==0)
		{
			Door_Status=1;
			TIM_SetCompare1(TIM3, 195);//����	 
		}
	}	
	EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);//����
	if(KEY0==0)	 //����KEY0
	{
		if(Led_Status==1)
		{
			Led_Status=0;
			LED0=0;//����	 
		}
		else if(Led_Status==0)
		{
			Led_Status=1;
			LED0=1;//�ص�	 
		}
	}		 
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
}
 
//�������ж�
//�ⲿ�ж�10������� 
void EXTI15_10_IRQHandler(void)
{
	DEBUG_LOG("�����жϳɹ�");
	delay_ms(10);//����
	if(EXTI_GetITStatus(EXTI_Line10) !=RESET)
	{
		count++;//���Ե��һ�´���������뼸���ж�
		DEBUG_LOG("�����жϴ�����%d\r\n",count);
		if(count>=10)
			count=0;
		//LCD_Clear(WHITE);
		EXTI_ClearITPendingBit(EXTI_Line10);  //���LINE4�ϵ��жϱ�־λ
		tp_dev.sta|=1<<7;//��Ǵ�����������
		if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))
		{
			tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff; 
		}
		DEBUG_LOG("�жϳɹ�����������Ϊ��x:%d,y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
//		if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>96&&tp_dev.y[0]<144)
//		{
//			LCD_Clear(WHITE);
//			Lcd_Page--;//LCD����ҳ��
//			if(Lcd_Page<0)
//				Lcd_Page=4;
//		}
//		if(tp_dev.x[0]>192&&tp_dev.x[0]<240 && tp_dev.y[0]>96&&tp_dev.y[0]<144)
//		{
//			LCD_Clear(WHITE);
//			Lcd_Page++;//LCD����ҳ��
//			if(Lcd_Page>4)
//				Lcd_Page=0;
//		}
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{
			//LCD_Clear(WHITE);
			//tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
			if(Lcd_Page>=0 && Lcd_Page<=5)//ֻ�����л�����ҳ���ʱ����������ã�Ŀǰһ����5��ģ�飩
			{
				changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
				if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//����������
				{
					//LCD_Clear(WHITE);
					Lcd_Page--;//LCD����ҳ��
					if(Lcd_Page<0)
						Lcd_Page=5;
				}
				else if(tp_dev.x[0]>192&&tp_dev.x[0]<240 && tp_dev.y[0]>96&&tp_dev.y[0]<144)//��������Ҽ�
				{
					//LCD_Clear(WHITE);
					Lcd_Page++;//LCD����ҳ��
					if(Lcd_Page>5)
						Lcd_Page=0;
				}
				else if(tp_dev.x[0]>56&&tp_dev.x[0]<184 && tp_dev.y[0]>96&&tp_dev.y[0]<224)//���ѡ�ж�Ӧ����ҳ��
				{
					if(Lcd_Page==0)
					{
						//LCD_Clear(WHITE);�������������жϲ�������
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
			if(Lcd_Page==8)//����ҳ�����ֵ�Ӽ�
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
			}
			else if(Lcd_Page==9)
			{
				if(tp_dev.x[0]>160&&tp_dev.x[0]<192 && tp_dev.y[0]>64&&tp_dev.y[0]<96)//���������ť
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
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//������ҳ��
				{
					//LCD_Clear(WHITE);
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					Lcd_Page=3;
				}
			}
			else if(Lcd_Page==7)//���ֲ��Ž���
			{
				if(tp_dev.x[0]>24&&tp_dev.x[0]<56 && tp_dev.y[0]>16&&tp_dev.y[0]<48)//������
				{
					if(Music_Voice>=2)//�˴�����һ��BUG,���ж�����Ϊif(Music_Voice>=1)ʱ������������0ʱ����������ɫ����
					{
						musicclear[0]=1;//��������ɫ��־λ
						//LCD_Fill(58,26,54+Music_Voice*4,38,WHITE);//��������ɫ
						Music_Voice--;
						//���к���������ͨ��������MP3ģ�鷢��ָ��
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
					}
				}
				else if(tp_dev.x[0]>90&&tp_dev.x[0]<150 && tp_dev.y[0]>216&&tp_dev.y[0]<276)//����/��ͣ�����л�
				{
					musicclear[1]=1;//����/ֹͣ������ɫ��־λ
					if(Play_flag==0)
					{ 
						//LCD_Fill(90,216,150,276,WHITE);//��������ɫ
						Play_flag=1;//�������֣���ʾ����ͼ��(��lcd.c��������л�)
					}
					else
					{
						//LCD_Fill(90,216,150,276,WHITE);//��������ɫ
						Play_flag=0;//��ͣ���ţ���ʾ��ͣͼ��(��lcd.c��������л�)
					}
				}
				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//������ҳ��
				{
					//LCD_Clear(WHITE);
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					Lcd_Page=1;
				}
			}
			else if(Lcd_Page==10)//ģʽѡ���������
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
			else if(Lcd_Page==6)//�������ý���
			{
//				if(tp_dev.x[0]>16&&tp_dev.x[0]<48 && tp_dev.y[0]>16&&tp_dev.y[0]<38)//ʱ��
//				{
//					AlarmClock[0][0]++;
//					if(AlarmClock[0][0]>23)
//						AlarmClock[0][0]=0;
//				}
//				else if(tp_dev.x[0]>16&&tp_dev.x[0]<48 && tp_dev.y[0]>66&&tp_dev.y[0]<88)//ʱ��
//				{
//					AlarmClock[0][0]--;
//					if(AlarmClock[0][0]<0)
//						AlarmClock[0][0]=23;
//				}
//				else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>16&&tp_dev.y[0]<38)//����
//				{
//					AlarmClock[0][1]++;
//					if(AlarmClock[0][1]>59)
//						AlarmClock[0][1]=0;
//				}
//				else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>66&&tp_dev.y[0]<88)//�ּ�
//				{
//					AlarmClock[0][1]--;
//					if(AlarmClock[0][1]<0)
//						AlarmClock[0][1]=59;
//				}
				//���ѡ���������Ӱ�ť(���Ҵ��ڴ����õ�����)��tp_dev.y[0]<272�����Ƿ�ֹ�����ʱ�ӷ��ų�ͻ
				if(tp_dev.x[0]>16 && tp_dev.x[0]<228 && tp_dev.y[0]>16 && tp_dev.y[0]<272 && Clock_Num>0)
				{
					int k=0;
					for(k=0;k<Clock_Num;k++)
					{
						if(tp_dev.x[0]<180 && tp_dev.y[0]>16 && tp_dev.y[0]<248)//���������ǼӼ�ʱ������
						{
							if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//ʱ��
							{
								AlarmClock[k][0]++;
								if(AlarmClock[k][0]>23)
									AlarmClock[k][0]=0;
							}
							else if(tp_dev.x[0]>16 && tp_dev.x[0]<48 && tp_dev.y[0]>(66+k*80) && tp_dev.y[0]<(88+k*80))//ʱ��
							{
								AlarmClock[k][0]--;
								if(AlarmClock[k][0]<0)
									AlarmClock[k][0]=23;
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(16+k*80) && tp_dev.y[0]<(38+k*80))//����
							{
								AlarmClock[k][1]++;
								if(AlarmClock[k][1]>59)
									AlarmClock[k][1]=0;
							}
							else if(tp_dev.x[0]>56&&tp_dev.x[0]<88 && tp_dev.y[0]>(66+k*80) &&tp_dev.y[0]<(88+k*80))//�ּ�
							{
								AlarmClock[k][1]--;
								if(AlarmClock[k][1]<0)
									AlarmClock[k][1]=59;
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
						}
					}
				} 
				else if(tp_dev.y[0]>272&&tp_dev.y[0]<320)//������/ɾ�����Ӱ�ť
				{
					if(tp_dev.x[0]>120&&tp_dev.x[0]<168)//���µ���ɾ����ť
					{
						//int j=0;
						Clock_Num--;
						if(Clock_Num<0)//��ֹԽ��
							Clock_Num=0;
						clockclear[3]=1;//��lcd.c��ǵ���˶�Ӧ���Ӱ�ť��lcd.c�������ʾ�������ж�Ӧ����������
						//LCD_Fill(0,16+Clock_Num*80,240,88+Clock_Num*80,WHITE);//ɾ������Ҫ���������Ӽ�¼������������ݣ�
						AlarmClock[Clock_Num][0]=0;
						AlarmClock[Clock_Num][1]=0;
						AlarmClock[Clock_Num][2]=0;//ɾ�����Ӻ�ͬ��Ҫɾ����ʱ�估ѡ�б�־λ��¼
					}
					else if(tp_dev.x[0]>184&&tp_dev.x[0]<232)//���µ�����Ӱ�ť
					{
						Clock_Num++;
						if(Clock_Num>3)//��ֹ������Խ�磬�����������3������
							Clock_Num=3;
					}
					else if(tp_dev.x[0]>0&&tp_dev.x[0]<48)//���ؽ���ǰ��ҳ�棬��Ϊ�������������/ɾ��ͬһ�У����Դ�����ڴ˴�ͬ����
					{
						//LCD_Clear(WHITE);
						changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
						Lcd_Page=0;
					}
				}
//				else if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//���ؽ���ǰ��ҳ��
//				{
//					LCD_Clear(WHITE);
//					Lcd_Page=0;
//				}
			}
			else if(Lcd_Page==11)//����ʱ����ʾ����
			{
				  if(tp_dev.x[0]>0&&tp_dev.x[0]<48 && tp_dev.y[0]>272&&tp_dev.y[0]<320)//���ؽ���ǰ��ҳ��
				{
					//LCD_Clear(WHITE);
					changeflag=1;//ҳ���л�������־λ,������жϺ�����LCD_Clear(WHITE)�������õ�BUG
					Lcd_Page=5;
				}
			}
		} 
	}
	EXTI_ClearITPendingBit(EXTI_Line10);  //���LINE4�ϵ��жϱ�־λ
}

