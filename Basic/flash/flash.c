#include "flash.h"




void FLASH_W(u32 addr, u16 data){
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//Clears all flags
		FLASH_ErasePage(addr);
		FLASH_ProgramHalfWord(addr, data); //��ָ����FLASHѡ���ֽڵ�ַ��д�����  ��Ϊд��������� u16�� �����u32�����FLASH_ProgramWord
		FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//Clears all flags
		FLASH_Lock();
}

u16 FLASH_R(u32 addr){
		u16 a;
		a = *(u16*)(addr);
		return a;

}



//void ALFLASH_W(u8 a, u8 d1, u8 d2){
//	u16 t;
//	t=d1*0x100+d2;
//	FLASH_W(FLASH_START_ADDR+a, t);
//}


//д�����ӵ�FLASH����
void ALFLASH_W(u8 *d){
	u8 i=0;
	u16 t=0;
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//Clears all flags
	FLASH_ErasePage(FLASH_START_ADDR);//����ָ����ַҳ
	for(i=0;i<10;i++){
		t=d[2*i]*0x100+d[2*i+1];
		FLASH_ProgramHalfWord(FLASH_START_ADDR+2*i, t);//��ָ����FLASHѡ���ֽڵ�ַ��д�����  ��Ϊд��������� u16�� �����u32�����FLASH_ProgramWord
		FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//Clears all flags
	}
	
	FLASH_Lock();//����FLASH
	
}







