#ifndef __TM1640_H
#define __TM1640_H


#include "sys.h"
#include "delay.h"

#define TM1640_GPIOPORT	GPIOA
#define TM1640_DIN			GPIO_Pin_12
#define TM1640_SCLK			GPIO_Pin_11


#define TM1640_LEDPART	0xc8


void TM1640_Init(void);//≥ı ºªØ
void TM1640_led(u8 date);
void TM1640_display(u8 address,u8 date);
void TM1640_display_add(u8 address,u8 *date);




#endif


