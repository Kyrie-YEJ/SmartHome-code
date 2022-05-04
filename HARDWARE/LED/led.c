#include"led.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE); //?? PB,PE ????
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //LED0-->PB.5 ????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_5); //PB.5 ???
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //LED1-->PE.5 ????
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_5); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //LED1-->PE.5 ????
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5); 
}
