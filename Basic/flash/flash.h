#ifndef __FLASH_H
#define __FLASH_H

#include "sys.h"


#define FLASH_START_ADDR  0x0801f000	  //写入的起始地址




void FLASH_W(u32 addr, u16 data);
u16 FLASH_R(u32 addr);

//写入闹钟的FLASH数据
void ALFLASH_W(u8 *d);

//写入温度的FLASH数据
void TMFLASH_W(u8 *d);

#endif

