#ifndef __FLASH_H
#define __FLASH_H

#include "sys.h"


#define FLASH_START_ADDR  0x0801f000	  //写入的起始地址




void FLASH_W(u32 addr, u16 data);
u16 FLASH_R(u32 addr);

void ALFLASH_W(u8 a, u8 d1, u8 d2);

#endif

