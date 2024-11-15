


#include "lm75a.h"

#include "i2c.h"


/*
读出LM75A的温度值（-55~125摄氏度）
温度正负（0正1负），温度整数， 温度小数点后两位， 依次放入*TempBuffer(十进制)
*/
void LM75A_GetTemp(u8 *TempBuffer){
	u8 buf[2]; //存储温度值
	u8 t=0, a=0;
	//I2C_READ_BUFFER(LM75A_ADD, 0x00, buf, 2);
	I2C_Read_Buffer(LM75A_ADD, 0x00, buf, 2);
	t=buf[0];
	*TempBuffer = 0; //判断温度值，0为正
	if(t&0x80){
		*TempBuffer = 1;//温度为负
		t=~t;//取反
		t++;//加1 得到补码
	}
	if(t&0x01){a=a+1;}
	if(t&0x02){a=a+2;}
	if(t&0x04){a=a+4;}
	if(t&0x08){a=a+8;}
	if(t&0x10){a=a+16;}
	if(t&0x20){a=a+32;}
	if(t&0x40){a=a+64;}
	TempBuffer++;
	*TempBuffer=a;
	//初始化两个变量，处理小数部分
	a=0;
	t=buf[1];
	if(t&0x20) {a=a+12;}
	if(t&0x40) {a=a+25;}
	if(t&0x80) {a=a+50;}
	TempBuffer++;
	*TempBuffer=a;
	
	
}




void LM75A_PowerDown(void){
	I2C_SEND_BYTE(LM75A_ADD, 0x01, 1);

}




















































