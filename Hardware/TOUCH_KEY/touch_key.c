



#include "touch_key.h"
#include "buzzer.h"

u8 KEY_VALUE = 0;

void TOUCH_KEY_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能外设时钟
	GPIO_InitTypeDef	GPIO_InitStructure; //定义gpio的初始化枚举结构
	GPIO_InitStructure.GPIO_Pin = TOUCH_KEY_A |TOUCH_KEY_B |TOUCH_KEY_C |TOUCH_KEY_D;//选择端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//选择上拉输入模式
	GPIO_Init(TOUCH_KEYPORT, &GPIO_InitStructure);
}


//1级菜单
void  Read_Key_Value(void){
	if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
		KEY_VALUE = 0;
		BUZZER_BEEP1();
		while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
	}
	if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
		KEY_VALUE = 1;
		BUZZER_BEEP1();
		while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
	}
	if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
		KEY_VALUE = 2;
		BUZZER_BEEP1();
		while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
	}
	if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
		KEY_VALUE = 3;
		BUZZER_BEEP1();
		while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
	}
}












