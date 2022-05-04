#include "beep.h"
void BEEP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //?? PB,PE
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //LED0-->PB.5 ????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8); //PB.5 ???
}
