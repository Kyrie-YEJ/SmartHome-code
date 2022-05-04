#include "key.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"	

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//按键扫描，mode 0：单按，1：可连按
u8 key_scan(u8 mode)
{
	static u8 key_up=1;
	if(mode)
		key_up=1;//支持连按
	if(key_up && (KEY0==0 || KEY1==0 || WK_UP==1))
	{
		key_up=0;
		if(KEY0==0) return KEY0_PRES;
		else if(KEY1==0) return KEY1_PRES;
		else if(WK_UP==1) return WKUP_PRES;
	}
	else if(KEY0==1 && KEY1==1 && WK_UP==0)
		key_up=1;
	return 0;
}
