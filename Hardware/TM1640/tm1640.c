


#include "tm1640.h"



#define DEL		1

//地址模式的设置
#define TM1640MEDO_ADD		0x40 //宏定义自动加一模式

#define TM1640MEDO_FIX		0x44		//宏定义 固定地址模式

//显示亮度的设置
//#define TM1640MEDO_DISPLAY  0x88   //宏定义 亮度  最小
//#define TM1640MEDO_DISPLAY  0x89   //宏定义 亮度
//#define TM1640MEDO_DISPLAY  0x8a   //宏定义 亮度
//#define TM1640MEDO_DISPLAY  0x8b   //宏定义 亮度
//#define TM1640MEDO_DISPLAY  0x8d   //宏定义 亮度
//#define TM1640MEDO_DISPLAY  0x8f   //宏定义 亮度 最大

#define TM1640MEDO_DISPLAY		0x8c
#define TM1640MEDO_DISPLAY_OFF		0x80


/*通信时序  启始（基础GPIO操作）*/
void TM1640_start(){
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(1)); //输出高电平
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(1)); 
	delay_us(DEL);
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(0));
	delay_us(DEL);
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(0));
	delay_us(DEL);
}

void TM1640_stop(){
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(0));
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(1));
	delay_us(DEL);
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(1));
	delay_us(DEL);
}



void TM1640_write(u8 date){
	u8 i;
	u8 aa;
	aa = date;
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(0));
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(0));
	for(i=0;i<8;i++){
		GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(0));
		delay_us(DEL);
		if(aa&0x01){
			GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(1));
			delay_us(DEL);
		}else{
			GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(0));
			delay_us(DEL);
		}
		GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(1));
		delay_us(DEL);
		aa=aa>>1;
	}
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(0));
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(0));

}




void TM1640_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = TM1640_DIN | TM1640_SCLK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TM1640_GPIOPORT, &GPIO_InitStructure);
	
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_SCLK, (BitAction)(1));
	GPIO_WriteBit(TM1640_GPIOPORT, TM1640_DIN, (BitAction)(1)); 
	TM1640_start();
	TM1640_write(TM1640MEDO_ADD);
	TM1640_stop();
	TM1640_start();
	TM1640_write(TM1640MEDO_DISPLAY);
	TM1640_stop();
}

void TM1640_led(u8 date){
	TM1640_start();
	TM1640_write(TM1640_LEDPART);  //有八个led当作第九个数码管
	TM1640_write(date);
	TM1640_stop();
}

void TM1640_display(u8 address, u8 date){
	const u8 buff[30]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,0x00, 0x40, 0x58,  0x09, 0x6d, 0x79, 0x07, 0x31, 	0x77,  0x38};//数字0~9及0~9加点显示段码表
    //---------------   0    1    2    3    4    5    6    7    8    9    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   无  - c(摄氏度)   :   S     E      T的左右半边    A     L
	TM1640_start();
	TM1640_write(0xc0+address);
	TM1640_write(buff[date]);
	TM1640_stop();
		
}


void TM1640_display_add(u8 address,u8 *date){	//地址自动加一模式的显示输出 
	u8 i;
 	const u8 buff[22]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,0x00, 0x40};//数字0~9及0~9加点显示段码表
    //---------------   0    1    2    3    4    5    6    7    8    9    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   无   -     
  TM1640_start();
   TM1640_write(0xC0+address);	         //设置起始地址
   for(i=0;i<8;i++){
      TM1640_write(buff[*(date+i)]); 
   }
   TM1640_stop(); 
}




















