#ifndef __TOUCH_KEY_H
#define __TOUCH_KEY_H

#include "sys.h"


#define TOUCH_KEYPORT	GPIOA
#define TOUCH_KEY_A		GPIO_Pin_0
#define TOUCH_KEY_B		GPIO_Pin_1
#define TOUCH_KEY_C		GPIO_Pin_2
#define TOUCH_KEY_D		GPIO_Pin_3

extern u8 KEY_VALUE;

void TOUCH_KEY_Init(void);//≥ı ºªØ

void Read_Key_Value(void);


#endif
