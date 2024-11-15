#include "key.h"



/*微动开关初始化*/
void KEY_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure; //定义GPIO的初始化枚举结构
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能或者失能 APB2 外设时钟
	GPIO_InitStructure.GPIO_Pin = KEY1 | KEY2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //选择IO接口工作方式， 上拉输入
	GPIO_Init(KEYPORT, &GPIO_InitStructure);//根据 GPIO_InitStruct 中指定的参数初始化外设 GPIOx 寄存器
}
