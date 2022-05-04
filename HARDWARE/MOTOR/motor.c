#include "motor.h"
#include "delay.h"
#include "oled.h"
/******************************************************/

/*******************************************************/
/*
���������
	MOTOR_IN0 PG3
	MOTOR_IN1 PG4
	MOTOR_IN2 PG5
	MOTOR_IN3 PG6
*/
/*******************************************************/
u16 MOTORRUN=64*64;
u8 codeCCW[8]={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09};   //��ʱ����ת�����
u8 codeCW[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};    //��ʱ����ת�����
extern u8 lastmove;//��һ�δ����������Ĭ�ϳ�ʼ״̬Ϊ�� 1���� 0����

void MOTOR_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PC�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOG, &GPIO_InitStructure);					 //�����趨������ʼ��
 GPIO_ResetBits(GPIOG,GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3);						 //�����
}

/****************************************************��ʱ����************************************/
void Timer2_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	// NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_Cmd(TIM2,DISABLE);
}


void SetMotor(unsigned char InputData)
{ 
	if(InputData&0x01)
		MOTOR_IN0=1;
	else
		MOTOR_IN0=0;
	if(InputData&0x02)
		MOTOR_IN1=1;
	else
		MOTOR_IN1=0;
	if(InputData&0x04)
		MOTOR_IN2=1;
	else
		MOTOR_IN2=0;
	if(InputData&0x08)
		MOTOR_IN3=1;
	else
		MOTOR_IN3=0;
}

//�������ֹͣ����
void Motor_Stop(void)
{
	MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=0;
}

u16 MotorStepCount=0;
//�������
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{ 
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIM3�����жϱ�־
		MotorStepCount++;
		if(lastmove==1)//����һ�δ������򿪣���ش��������˳ʱ��ת��
		{
			SetMotor(codeCW[MotorStepCount%8]);
			if(MotorStepCount>MOTORRUN)
			{
				MotorStepCount=0;
				Motor_Stop();
				TIM_Cmd(TIM2, DISABLE);
				lastmove=0;//�Ѵ������ر�־��0����ʾĿǰ�������ڹر�״̬
			}
		}
		else if(lastmove==0)//����һ�δ������رգ��򿪴����������ʱ��ת��
		{
			SetMotor(codeCCW[MotorStepCount%8]);
			if(MotorStepCount>MOTORRUN)
			{
				MotorStepCount=0;
				Motor_Stop();
				TIM_Cmd(TIM2, DISABLE);
				lastmove=1;//�Ѵ������ر�־��1����ʾĿǰ�������ڴ�״̬
			}
		}
	}
}

/****************************************************��ʱ����************************************/









/***************************************************�Ƕ�ʱ����*******************************/
//���������������2��     dir:1��ʾ�����ת��0��ʾ��ת
void Motor_Move(u8 dir)
{
	if(dir==1)
	{
		MOTOR_IN0=1;
		delay_ms(3);
		MOTOR_IN0=0;
		MOTOR_IN1=1;
		delay_ms(3);
		MOTOR_IN1=0;
		MOTOR_IN2=1;
		delay_ms(3);
		MOTOR_IN2=0;
		MOTOR_IN3=1;
		delay_ms(3);
		MOTOR_IN3=0;
	}
	else
	{
		MOTOR_IN3=1;
		delay_ms(3);
		MOTOR_IN3=0;
		MOTOR_IN2=1;
		delay_ms(3);
		MOTOR_IN2=0;
		MOTOR_IN1=1;
		delay_ms(3);
		MOTOR_IN1=0;
		MOTOR_IN0=1;
		delay_ms(3);
		MOTOR_IN0=0;
	}
}
//���������������2��     dir:1��ʾ�����ת��0��ʾ��ת��usΪ��ʱ�������ɵ���
void Motor_Move2(u8 dir,u32 us)
{
	if(dir)
	{
		MOTOR_IN0=1;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=0;//A
		delay_us(us);
		//MOTOR_IN0=1;
		MOTOR_IN0=1;MOTOR_IN1=1;MOTOR_IN2=0;MOTOR_IN3=0;//AB
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=1;MOTOR_IN2=0;MOTOR_IN3=0;//B
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=1;MOTOR_IN2=1;MOTOR_IN3=0;//BC
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=1;MOTOR_IN3=0;//C
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=1;MOTOR_IN3=1;//CD
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=1;//D
		delay_us(us);
		MOTOR_IN0=1;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=1;//DA
		delay_us(us);
	}
	else
	{
		MOTOR_IN0=1;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=1;//DA
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=1;//D
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=1;MOTOR_IN3=1;//CD
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=1;MOTOR_IN3=0;//C
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=1;MOTOR_IN2=1;MOTOR_IN3=0;//BC
		delay_us(us);
		MOTOR_IN0=0;MOTOR_IN1=1;MOTOR_IN2=0;MOTOR_IN3=0;//B
		delay_us(us);
		MOTOR_IN0=1;MOTOR_IN1=1;MOTOR_IN2=0;MOTOR_IN3=0;//AB
		delay_us(us);
		MOTOR_IN0=1;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=0;//A
		delay_us(us);
	}
}

