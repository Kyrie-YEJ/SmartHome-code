#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

void Timer2_Init(u16 arr,u16 psc);
void Timer3_Init(u16 arr,u16 psc);
void Timer5_Init(u16 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM2_PWM_Init(u16 arr,u16 psc);
#endif
