

#include "buzzer.h"
#include "delay.h"

/*初始化*/
void BUZZER_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = BUZZER;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BUZZERPORT, &GPIO_InitStructure);
	
	GPIO_WriteBit(BUZZERPORT, BUZZER, (BitAction)(1)); //默认输出高电平，蜂鸣器电路使用PNP型三极管，电路不导通；
}




/*蜂鸣*/
void BUZZER_BEEP1(void){
	u16 i;
	for(i=0; i<200; i++){
		GPIO_WriteBit(BUZZERPORT, BUZZER, (BitAction)(0));
		delay_us(500);
		GPIO_WriteBit(BUZZERPORT, BUZZER, (BitAction)(1));
		delay_us(500);
	}

}


void BUZZER_BEEP2(void){ //蜂鸣器响一声（开机音）
	u16 i;
	for(i=0;i<400;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(800); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(800); //延时		
	}
}
void BUZZER_BEEP3(void){ //蜂鸣器响一声（进入调时音）
	u16 i;
	for(i=0;i<150;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(200); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(200); //延时		
	}
	for(i=0;i<100;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(300); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(300); //延时		
	}
	for(i=0;i<250;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(100); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(100); //延时		
	}
}
void BUZZER_BEEP4(void){ //蜂鸣器响一声（退出调时音）
	u16 i;
	for(i=0;i<450;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(200); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(200); //延时		
	}
	for(i=0;i<250;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(400); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(400); //延时		
	}
	for(i=0;i<450;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(100); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(100); //延时		
	}
}









uc16 music1[78]={ //音乐1的数据表（奇数是音调，偶数是长度） uc16 无符号的只读变量，只读变量存储在flash里
330,750,
440,375,
494,375,
523,750,
587,375,
659,375,
587,750,
494,375,
392,375,
440,1500,
330,750,
440,375,
494,375,
523,750,
587,375,
659,375,
587,750,
494,375,
392,375,
784,1500,
659,750,
698,375,
784,375,
880,750,
784,375,
698,375,
659,750,
587,750,
659,750,
523,375,
494,375,
440,750,
440,375,
494,375,
523,750,
523,750,
494,750,
392,750,
440,3000
};


void MIDI_PLAY(void){ //MIDI音乐
	u16 i,e;
	for(i=0;i<39;i++){
		for(e=0;e<music1[i*2]*music1[i*2+1]/1000;e++){
			GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
			delay_us(500000/music1[i*2]); //延时		
			GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
			delay_us(500000/music1[i*2]); //延时	
		}	
	}
}







