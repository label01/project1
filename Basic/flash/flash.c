#include "flash.h"




void FLASH_W(u32 addr, u16 data){
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//Clears all flags
		FLASH_ErasePage(addr);
		FLASH_ProgramHalfWord(addr, data); //在指定的FLASH选择字节地址编写半个字  因为写入的数据是 u16， 如果是u32则调用FLASH_ProgramWord
		FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//Clears all flags
		FLASH_Lock();
}

u16 FLASH_R(u32 addr){
		u16 a;
		a = *(u16*)(addr);
		return a;

}



void ALFLASH_W(u8 a, u8 d1, u8 d2){
	u16 t;
	t=d1*0x100+d2;
	FLASH_W(FLASH_START_ADDR+a, t);
}









