#include "motor.h"
#include "delay.h"
#include "oled.h"
/******************************************************/

/*******************************************************/
/*
步进电机：
	MOTOR_IN0 PG3
	MOTOR_IN1 PG4
	MOTOR_IN2 PG5
	MOTOR_IN3 PG6
*/
/*******************************************************/
u16 MOTORRUN=64*64;
u8 codeCCW[8]={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09};   //逆时针旋转相序表
u8 codeCW[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};    //正时针旋转相序表
extern u8 lastmove;//上一次窗户打开情况，默认初始状态为关 1：开 0：关

void MOTOR_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //使能PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOG, &GPIO_InitStructure);					 //根据设定参数初始化
 GPIO_ResetBits(GPIOG,GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3);						 //输出低
}

/****************************************************定时器版************************************/
void Timer2_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	// NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
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

//步进电机停止函数
void Motor_Stop(void)
{
	MOTOR_IN0=0;MOTOR_IN1=0;MOTOR_IN2=0;MOTOR_IN3=0;
}

u16 MotorStepCount=0;
//步进电机
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{ 
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIM3更新中断标志
		MotorStepCount++;
		if(lastmove==1)//若上一次窗户被打开，则关窗户，电机顺时针转动
		{
			SetMotor(codeCW[MotorStepCount%8]);
			if(MotorStepCount>MOTORRUN)
			{
				MotorStepCount=0;
				Motor_Stop();
				TIM_Cmd(TIM2, DISABLE);
				lastmove=0;//把窗户开关标志置0，表示目前窗户处于关闭状态
			}
		}
		else if(lastmove==0)//若上一次窗户被关闭，则开窗户，电机逆时针转动
		{
			SetMotor(codeCCW[MotorStepCount%8]);
			if(MotorStepCount>MOTORRUN)
			{
				MotorStepCount=0;
				Motor_Stop();
				TIM_Cmd(TIM2, DISABLE);
				lastmove=1;//把窗户开关标志置1，表示目前窗户处于打开状态
			}
		}
	}
}

/****************************************************定时器版************************************/









/***************************************************非定时器版*******************************/
//步进电机驱动函数2：     dir:1表示电机正转；0表示反转
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
//步进电机驱动函数2：     dir:1表示电机正转，0表示反转；us为延时参数，可调速
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

