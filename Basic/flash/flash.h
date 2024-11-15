#ifndef __FLASH_H
#define __FLASH_H

#include "sys.h"

void FLASH_W(u32 addr, u16 data);
u16 FLASH_R(u32 addr);



#endif

