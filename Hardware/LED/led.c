/*
日期：2024.08.15

作者：label


*/


#include "led.h"

//Led灯初始化
void LED_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED1 | LED2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}




/*呼吸灯*/
void LED_Breathe(FunctionalState NewState){
	/*呼吸灯配置变量*/
	u8 i;
	static u8	menu = 0;
  static u16 t = 1;
	//初始化
	/*呼吸灯定义变量初始化*/
	i = 0;
	if (!NewState){
		GPIO_Write(GPIOB, 0x0000);
		menu = 0;
		t = 1;
		return;
	}
	/*变亮循环*/
	if (menu == 0){
			for(i = 0; i < 10; i++){
				GPIO_SetBits(GPIOB, LED1);
				GPIO_ResetBits(GPIOB, LED2);
				delay_us(t);
				GPIO_ResetBits(GPIOB, LED1);
				GPIO_SetBits(GPIOB, LED2);
				delay_us(501-t);
			}
			t++;
			if(t == 500){
				menu = 1;
			}
		}
		
		/*变暗循环*/
		if (menu == 1){
			for(i = 0; i < 10; i++){
				GPIO_SetBits(GPIOB, LED1);
				GPIO_ResetBits(GPIOB, LED2);
				delay_us(t);
				GPIO_ResetBits(GPIOB, LED1);
				GPIO_SetBits(GPIOB, LED2);
				delay_us(501-t);
			}
			t--;
			if(t == 1){
				menu = 0;
			}
		}
}



