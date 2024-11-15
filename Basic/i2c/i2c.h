
#ifndef __I2C_H
#define __I2C_H

#include "sys.h"

#define I2CPORT	GPIOB
#define I2C_SCL	GPIO_Pin_6
#define I2C_SDA	GPIO_Pin_7


#define HostAddress		0xc0 //总线主机的器件地址， 可自定义，但需要避免和从机地址冲突
#define BusSpeed		200000 //总线速度


void I2C_Configuration(void);
void I2C_SEND_BUFFER(u8 SlaveAddr, u8 WriteAddr, u8* pBuffer, u16 NumByteToWrite);
void I2C_SEND_BYTE(u8 SlaveAddr,u8 writeAddr,u8 pBuffer);
void I2C_READ_BUFFER(u8 SlaveAddr,u8 readAddr,u8* pBuffer,u16 NumByteToRead);
u8 I2C_READ_BYTE(u8 SlaveAddr,u8 readAddr);

void I2C_Send_Byte(u8 SlaveAddr, u8 WriteAddr, u8 pBuffer);
void I2C_Read_Buffer(u8 SlaveAddr, u8 ReadAddr, u8 *pBuffer, u16 NumByteToRead);
u8 I2C_Read_Byte(u8 SlaveAddr, u8 ReadAddr);
void I2C_Send_Buffer(u8 SlaveAddr, u8 WriteAddr, u8 *pBuffer, u16 NumByteToWrite);

#endif



































