#include "remote.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "oled.h"
#include "led.h"
#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK��ӢSTM32������
//����ң�ؽ������� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//����ң�س�ʼ��
//����IO�Լ���ʱ��4�����벶��
void Remote_Init(void)    			  
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��PORTBʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 //PB9 ���� 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		//�������� 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//��ʼ��GPIOB.9
	
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000; //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // ѡ������� IC4ӳ�䵽TI4��
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
  TIM_ICInit(TIM4, &TIM_ICInitStructure);//��ʼ����ʱ�����벶��ͨ��

  TIM_Cmd(TIM4,ENABLE ); 	//ʹ�ܶ�ʱ��4
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	

	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC4IE�����ж�								 
}

//����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	  
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
    return sta;
}

u8 key_get=0;		//���ü�ֵ
char str[12];	
int n=-1;		//����ң������ʱ��λ
//static int x=3,y=0;//ʱ������
//int i,j,k=0;
extern u8 Led_Status;//LED�Ʊ�־λ
extern u8 Windows_Status;//�����򿪱�־
extern u8 Door_Status;//�Ŵ򿪱�־
extern u8 SR301_Status;//�Ƿ����˱�־
extern u8 Beep_Status;//�������־
extern int position;//OLED��>��������
extern u8 Mq135_Max;//������ֵ
extern u8 Temp_Max;//�¶���ֵ
extern u8 Humi_Max;//ʪ����ֵ
extern u8 Light_Max;//�����ֵ
extern u8 mode;
extern u8 ClearOled;//Ϊ�˽������ң�ص��µ�OLEDˢ��BUG���趨�ı�־λ
extern u8 HomeMode;//�Զ�ģʽ��־λ 0���ֶ�ģʽ 1���Զ�ģʽ 2:���ģʽ 3��˯��ģʽ

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
		else if(strcmp(str,"UP")==0)//����
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
				/*1������ʱ��(���ڸ�һ��) 2���鿴ʱ��*/
				if(position<0)
					position=1;
			}
		}
		else if(strcmp(str,"DOWN")==0)//����
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
				/*1������ʱ��(���ڸ�һ��) 2���鿴ʱ��*/
				if(position>1)
					position=0;
			}
		}
		else if(strcmp(str,"LEFT")==0)//����
		{
			//DEBUG_LOG("Key=LEFT\r\n");
		}
		else if(strcmp(str,"RIGHT")==0)//����
		{
			//DEBUG_LOG("Key=RIGHT\r\n");
		}
		else if(strcmp(str,"PLAY")==0)//ȷ�ϼ�
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
						TIM_SetCompare1(TIM3, 195);//����
					}
					else
					{
						Door_Status=0;
						TIM_SetCompare1(TIM3, 185);//����
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
		else if(strcmp(str,"VOL+")==0)//���Ӽ�
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
		else if(strcmp(str,"VOL-")==0)//���ټ�
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
		else if(strcmp(str,"DELETE")==0)//���ؼ�
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

//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		    	 
 
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;							//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)
			{
				RmtSta&=~(1<<7);//���������ʶ(��һ�ΰ���ֻ�ܷ���һ����Ϣ)
				RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			}
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					//���������ʶ
				RmtSta&=0XF0;						//��ռ�����	
			}								 	   	
		}							    
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{	  
		if(RDATA)//�����ز���
		{
  			TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling);						//CC4P=1	����Ϊ�½��ز���
			TIM_SetCounter(TIM4,0);							//��ն�ʱ��ֵ
			RmtSta|=0X10;							//����������Ѿ�������
		}else //�½��ز���
		{
			Dval=TIM_GetCapture4(TIM4);					//��ȡCCR4Ҳ������CC4IF��־λ
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising);				//CC4P=0	����Ϊ�����ز���
			if(RmtSta&0X10)							//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)//���յ���������
				{
					
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=0;					//���յ�0	   
					}else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=1;					//���յ�1
					}else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 					//������������1��
						RmtSta&=0XF0;				//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;					//��ǳɹ����յ���������
					RmtCnt=0;						//�����������������
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);
	remote_change_time();	
}


































