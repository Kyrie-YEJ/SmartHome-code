#ifndef __SR301_H
#define __SR301_H
#include "sys.h"

#define SR301_IN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)
void SR301_Init(void);
void EXTIX_SR301_Init(void);
#endif
