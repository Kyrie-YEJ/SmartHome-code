#include "sr301.h"
#include "delay.h"
#include "usart.h"

extern u8 leaveflag;//��ұ�־λ 0���ڼ� 1�����

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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

 //GPIOE.2	  �ж����Լ��жϳ�ʼ������ �½��ش��� 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 
}
extern u8 HomeMode;//�Զ�ģʽ��־λ 0���ֶ�ģʽ 1���Զ�ģʽ 2:���ģʽ 3��˯��ģʽ
void EXTI2_IRQHandler(void)
{
	delay_ms(10);
	if(SR301_IN)
	{
		if(HomeMode==2)
		{
			DEBUG_LOG("##################���������˴���##################\r\n");
			leaveflag=1;
		}
		//TIM_SetCompare1(TIM3, 185);//90��
	}
	EXTI_ClearITPendingBit(EXTI_Line2); //���LINE0�ϵ��жϱ�־λ 
}


