#ifndef __MOTOR_H
#define __MOTOR_H	 
#include "sys.h"

#define MOTOR_IN0 PGout(3)
#define MOTOR_IN1 PGout(4)
#define MOTOR_IN2 PGout(5)
#define MOTOR_IN3 PGout(6)

void MOTOR_Init(void);//IO≥ı ºªØ	
void Timer2_Init(u16 arr,u16 psc);
void Motor_Move(u8 dir);
void Motor_Move2(u8 dir,u32 us);
void Motor_Stop(void);
#endif
