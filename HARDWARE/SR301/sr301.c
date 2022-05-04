#include "sr301.h"
#include "delay.h"
#include "usart.h"

extern u8 leaveflag;//离家标志位 0：在家 1：离家

void SR301_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void EXTIX_SR301_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	SR301_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

 //GPIOE.2	  中断线以及中断初始化配置 下降沿触发 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//使能按键WK_UP所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
}
extern u8 HomeMode;//自动模式标志位 0：手动模式 1：自动模式 2:离家模式 3：睡觉模式
void EXTI2_IRQHandler(void)
{
	delay_ms(10);
	if(SR301_IN)
	{
		if(HomeMode==2)
		{
			DEBUG_LOG("##################警报：有人闯入##################\r\n");
			leaveflag=1;
		}
		//TIM_SetCompare1(TIM3, 185);//90度
	}
	EXTI_ClearITPendingBit(EXTI_Line2); //清除LINE0上的中断标志位 
}


