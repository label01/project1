












#include "usart.h"


//使USART串口可用printf函数发送
//在usart.h文件里可更换printf函数的串口号
#if 1
#pragma import(__use_no_semihosting)
struct __FILE{
	int handle;
};

FILE __stdout;

//空操作
_sys_exit(int x){
	x = x;
}

//重定义fpuc函数
int fputc(int ch, FILE *f){
	while((USART_n->SR&0x40)==0);//循环发送，知道发送完毕
	USART_n->DR = (u8) ch;
	return ch;
}




#endif

/*USART1 串口相关程序*/

#if EN_USART1 
u8 USART1_RX_BUF[USART1_REC_LEN];

u16 USART1_RX_STA=0;


/* 串口1的引脚 PA9 PA10 */
void USART1_Init(u32 bound){
	//先配置GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	//使能或失能APB2外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);
	//USART1_RX PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1_TX PA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ; //浮空输入
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//配置中断
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //串口1中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能IRQ通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //子优先级
	NVIC_Init(&NVIC_InitStructure);
	
	//USART1 初始化设置
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE); //USART_IT_RXNE: Receive Data register not empty interrupt
	USART_Cmd(USART1, ENABLE); //使能串口
	
}



	//以下是字符串接收到USART_RX_BUF[]的程序，(USART_RX_STA&0x3FFF)是数据的长度（不包括回车）
	//当(USART_RX_STA&0xC000)为真时表示数据接收完成，即超级终端里按下回车键。
	//在主函数里写判断if(USART_RX_STA&0xC000)，然后读USART_RX_BUF[]数组，读到0x0d 0x0a即是结束。
	//注意在主函数处理完串口数据后，要将USART_RX_STA清0
void USART1_IRQHandler(void){ //串口1中断服务程序（固定的函数名不能修改）	
	uint8_t Res; //顶一个变量用于接收数据
	if(USART_GetFlagStatus(USART1, USART_IT_RXNE) != RESET){//判断串口接收数据标志位非空
		Res = USART_ReceiveData(USART1); // 读取接收的数据
		printf("%c", Res); //把输入的数据打印在超级终端上，要不没有显示
		//开始处理接收数据
		/*
		USART1_RX_STA 是一个无符号的十六位全局变量
		bit15，	接收完成标志
		bit14，	接收到0x0d
		bit13~0，	接收到的有效字节数目
		*/
		if((USART1_RX_STA&0x8000) == 0){ //接收未完成
			if(USART1_RX_STA&0x4000){//接收到0x0d也就是回车字符
				if(Res != 0x0a)USART1_RX_STA=0; // 接收错误，0x0a是换行符，如果在接收到回车符之后不是换行符那就错误了，重新开始
				else USART1_RX_STA |= 0x8000; //给最高位置1，表示接收完成
			}else{//没有收到回车
				if(Res==0x0d)USART1_RX_STA|=0x4000;//给标志位置1
				else{
					USART1_RX_BUF[USART1_RX_STA&0x3fff] = Res;
					USART1_RX_STA++;//数据长度加1
					if(USART1_RX_STA > (USART1_REC_LEN-1))USART1_RX_STA = 0;// 接收长度大于缓冲区，重新开始接收
				}
				
			}
		}
	
	}
} 
#endif	




















