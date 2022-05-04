#ifndef __KEY_H
#define __KEY_H
#include "sys.h"
#define KEY0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
#define KEY1 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define WK_UP GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

#define KEY0_PRES 1
#define KEY1_PRES 2
#define WKUP_PRES 3
void KEY_Init(void);
u8 key_scan(u8 mode);
#endif
