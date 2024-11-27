/*
出品： 杜洋工作室 DoYoung Studio
程序名：
编写人： 杜洋
编写时间： 201 年月日
硬件支持： STM32F103C8 外部晶振 8M
修改日志：
1-
*/
#include "stm32f10x.h" //STM 头文件

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "rtc.h"
#include "touch_key.h"
#include "tm1640.h"
#include "lm75a.h"
#include "i2c.h"
#include "buzzer.h"
#include "flash.h"

#define KEY_SPEED1	100		//长按计时
#define KEY_SPEED2 100		//数值刷新间隔ms


int main(void) {//主程序
	
	
	u8 buffer[3]; //缓存温度值
	u8 MENU=0; //菜单值
	u8 MENU2=0; //二级菜单值
	u16 i=0; // 变量i
	u8 j=0; //变量j
	u16 k=0; //变量k， 用于控制闹钟响铃时间
	u8 a=0; //长按计数
	u16 b=0; //循环显示日期 时间 温度
	u16 c=0;//暂存闹钟值
	u8 d=0; //闹钟到时标志位
	u8 e=0; //闹钟到时闹钟号
	u8 tup=0;//更新时间标志位
	u8 altime[20]={0};//保存是个闹钟的时 分
	u8 temp[4]={0}; //保存温度的整数和小数
	RCC_Configuration();//系统时钟初始化
	LED_Init();//LED初始化
	TOUCH_KEY_Init();//触摸按键初始化
	
	RTC_Config();// RTC初始化
	TM1640_Init(); //TM1640初始化
	I2C_Configuration(); //i2c初始化
	TOUCH_KEY_Init(); //触摸按键初始化
	BUZZER_Init();//蜂鸣器初始化
	delay_ms(1000);//延时1S等待
	BUZZER_BEEP2();//开机提示音
	
	//读FLASH数据
	for(i=0; i<12; i++){//读出十组闹钟分钟数值+2组温度值
		c=FLASH_R(FLASH_START_ADDR+i*2);//从指定页的地址读FLASH
		if(i<10){
			altime[2*i]=c/0x100;
			altime[2*i+1]=c%0x100;
		}else{//温度值赋值
			temp[(i-10)*2]=c/0x100;
			temp[(i-10)*2+1]=c%0x100;
		}
	}
	
	//判断，如果闹钟0的小时值大于24就初始化闹钟的所有数据
	if(altime[0]>24){
		//初始化所有值
		for(i=0;i<10;i++){
			altime[2*i]=24;
			altime[2*i+1]=0;
		}
	}
	
	//判断，如果温度0的小时值大于55就初始化温度的所有数据
	if(temp[0]>55){
		//初始化所有值
		for(i=0;i<2;i++){
			temp[2*i]=0;
			temp[2*i+1]=0;
		}
	}
	
	//写入一遍闹钟的所有数据到FLASH里
	ALFLASH_W(altime);
	
	//写入一遍设置的温度值
	TMFLASH_W(temp);
	
	while (1) {
		//菜单每次循环都重新获得一遍键值
		MENU = KEY_VALUE;
		//tup，时间更新标志，判断是否设置时间
		if (tup == 1){
			tup=0;//时间更新标志复位
			RTC_Set(ryear, rmon, rday, rhour, rmin, rsec);//设置时间
		}
		//读取时间和环境信息在特定菜单下
		if(MENU < 3 || MENU > 20){
			//读取RTC时间
			RTC_Get();
			//读取环境温度值
			LM75A_GetTemp(buffer);
		}
		//日期时间温度循环显示计时
		b++;
		if(MENU<3){
			if(b==1) KEY_VALUE=1;
			if(b==3000) KEY_VALUE=0;
			if(b==5000) KEY_VALUE=2;
			if(b==7000) b=0;
			//menu菜单值大于3时b的值可能会大于7000
			if(b>7000) b=0;
		}
		
		//闹钟到时检测
		if(MENU<3 && d==0){//时钟正常显示且闹钟未触发
			for(i=0;i<10; i++){//循环检查所有闹钟是否到时
				if(altime[2*i]==rhour&&altime[2*i+1]==rmin &&rsec==0){
					d=1;//闹钟到时标志位置1触发
					e=i;//保存到时闹钟编号
					KEY_VALUE=20;//闹钟到时处理
				}
			}
		}
		
		
		if(d==1){//闹钟到时，响铃时间控制
			k++;//到时计数，控制响铃时间
			if(k>1000){
				KEY_VALUE=1;//响铃时间超时，返回时间菜单
				d=0;//闹钟到时标志位复位
				k=0;//到时计数归零
			}
		}
		
		
		//无限循环程序
		//年月日
		if (MENU==0){
			TM1640_display(0,ryear%100/10);	//年
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21); 
			TM1640_display(3,rmon/10);	//月
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21);
			TM1640_display(6,rday/10); //日
			TM1640_display(7,rday%10);
			Read_Key_Value();
			
			
		}
		//时分秒
		if (MENU==1){
			TM1640_display(0,rhour/10);//时	
			TM1640_display(1,rhour%10);
			TM1640_display(2,23); 
			TM1640_display(3,rmin/10);
			TM1640_display(4,rmin%10);	//分
			TM1640_display(5,23);
			TM1640_display(6,rsec/10); //秒
			TM1640_display(7,rsec%10);
			Read_Key_Value();
		}
		//显示温度值
		if (MENU==2){
			TM1640_display(0,20);
			TM1640_display(1,20);
			TM1640_display(2,20); 
			if (buffer[0])TM1640_display(3,21);
			else TM1640_display(3,20);
			TM1640_display(4,buffer[1]%100/10);	
			TM1640_display(5,buffer[1]%10+10);
			TM1640_display(6,buffer[2]%100/10);
			TM1640_display(7,22);
			Read_Key_Value();
		}
		//设置菜单
		if (MENU==3){
			
			TM1640_display(4,20);	
			TM1640_display(5,20);
			TM1640_display(6,20);
			TM1640_display(7,20);
			if(MENU2 == 0){
				//set
				TM1640_display(0,24);
				TM1640_display(1,25);
				TM1640_display(2,26); 
				TM1640_display(3,27);
			}
			if(0<MENU2 && MENU2<11){
				//al0~al9
				TM1640_display(0,28);
				TM1640_display(1,29);
				TM1640_display(2,MENU2-1); 
				TM1640_display(3,20);
			}
			if(MENU2>10){
				//tc0~tc1
				TM1640_display(0,32);
				TM1640_display(1,22);
				TM1640_display(2,MENU2-11); 
				TM1640_display(3,20);
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) {
				//二级菜单正向浏览
				MENU2++;
				if (MENU2>12) MENU2=0;
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				//二级菜单AL0~AL9 一共设置10个闹钟 + 2个温度值 二级菜单负向浏览
				if (MENU2==0) MENU2=13;
				MENU2--;
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE=0;//取消返回日期显示
				MENU2=0; //二级菜单复位
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
				
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				//确认跳转菜单
				if(MENU2 == 0){
					KEY_VALUE=4;
				}
				if(MENU2>0 && MENU2<11){
					KEY_VALUE=9+MENU2;
					j=2*(KEY_VALUE-10);//j是闹钟数组编号
				}
				else{
					KEY_VALUE=10+MENU2;
					j=2*(KEY_VALUE-21);//j是温度值数组编号
				}
				MENU2=0; //二级菜单复位
				BUZZER_BEEP3();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//设置年
		if (MENU == 4){
			//配置闪烁效果
			i++;
			if(i < 700){
				TM1640_display(0,ryear%100/10);	//年
				TM1640_display(1,ryear%100%10);
			}else{
				TM1640_display(0,20);	//年
				TM1640_display(1,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(2,21); 
			TM1640_display(3,rmon/10);	//月
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21);
			TM1640_display(6,rday/10); //日
			TM1640_display(7,rday%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) {
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						ryear++;
						if (ryear>2099) ryear=1970;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(0,ryear%100/10);	//年
						TM1640_display(1,ryear%100%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{//单击
					ryear++;
					if (ryear>2099) ryear=1970;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时	
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){//判断是否长按
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						ryear--;
						if (ryear<1970) ryear=1970;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(0,ryear%100/10);	//年 
						TM1640_display(1,ryear%100%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					ryear--;
					if (ryear<1970) ryear=1970;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 5;//跳转到下一级菜单
				tup=1;//时间更新标志位
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 0;//返回初始菜单
				tup=1;//时间更新标志位
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//设置月
		if (MENU == 5){
			//配置闪烁效果
			i++;
			if(i < 700){
				TM1640_display(3,rmon/10);	//月
				TM1640_display(4,rmon%10);
			}else{
				TM1640_display(3,20);	//月
				TM1640_display(4,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,ryear%100/10);	//年
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21); 
			TM1640_display(5,21);
			TM1640_display(6,rday/10); //日
			TM1640_display(7,rday%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rmon++;
						if (rmon>12) rmon=1;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(3,rmon/10);	//月
						TM1640_display(4,rmon%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					rmon++;
					if (rmon>12) rmon=1;
					BUZZER_BEEP1();//按键提示音
				}
			}
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						rmon--;
						if (rmon<1) rmon=12;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(3,rmon/10);	//月
						TM1640_display(4,rmon%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					rmon--;
					if (rmon<1) rmon=12;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 6;
				//时间更新标志位
				tup=1;//时间更新标志位置1，更新设置时间
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 0;//返回初始菜单
				tup=1;
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		
		//设置日
		if (MENU == 6){
			//配置闪烁效果
			i++;
			if(i < 700){
				TM1640_display(6,rday/10); //日
				TM1640_display(7,rday%10);
			}else{
				TM1640_display(6,20); //日
				TM1640_display(7,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,ryear%100/10);	//年
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21); 
			TM1640_display(3,rmon/10);	//月
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21);
			
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rday++;
						if (rday>31) rday=1;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(6,rday/10); //日
						TM1640_display(7,rday%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					rday++;
					if (rday>31) rday=1;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						rday--;
						if (rday<1) rday=31;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(6,rday/10); //日
						TM1640_display(7,rday%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					rday--;
					if (rday<1) rday=31;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 7;//跳转下一级菜单
				tup=1;//时间更新标志位
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 0;
				//时间更新标志位
				tup=1;
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//设置时
		if (MENU == 7){
			//配置闪烁效果
			i++;
			if(i < 700){
				TM1640_display(0,rhour/10);//时	
				TM1640_display(1,rhour%10);
			}else{
				TM1640_display(0,20);	//时
				TM1640_display(1,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(2,23); 
			TM1640_display(3,rmin/10);
			TM1640_display(4,rmin%10);	//分
			TM1640_display(5,23);
			TM1640_display(6,rsec/10); //秒
			TM1640_display(7,rsec%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按累加
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rhour++;
						if (rhour>23) rhour=0;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(0,rhour/10);//时	
						TM1640_display(1,rhour%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					rhour++;
					if (rhour>23) rhour=0;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按累减
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if (rhour==0) rhour=24;	
						rhour--;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(0,rhour/10);//时	
						TM1640_display(1,rhour%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					if (rhour==0) rhour=24;	
					rhour--;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 8;//跳转下一级菜单
				tup=1;//时间更新标志位
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 1;//跳转到时间菜单
				tup=1;//时间更新标志位
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//设置分钟
		if (MENU == 8){
			//配置闪烁效果
			i++;
			if(i < 700){
				TM1640_display(3,rmin/10);
				TM1640_display(4,rmin%10);	//分
			}else{
				TM1640_display(3,20);	//分
				TM1640_display(4,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,rhour/10);//时	
			TM1640_display(1,rhour%10);
			TM1640_display(2,23); 
			TM1640_display(5,23);
			TM1640_display(6,rsec/10); //秒
			TM1640_display(7,rsec%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rmin++;
						if (rmin>59) rmin=0;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(3,rmin/10);
						TM1640_display(4,rmin%10);	//分
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					rmin++;
					if (rmin>59) rmin=0;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按累减
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if (rmin==0) rmin=60;				
						rmin--;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(3,rmin/10);
						TM1640_display(4,rmin%10);	//分
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					if (rmin==0) rmin=60;				
					rmin--;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 9;//跳转下一级菜单
				tup=1;//时间更新标志位
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 1;//返回时间显示
				tup=1;//时间更新标志位
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//设置秒数
		if (MENU == 9){
			//配置闪烁效果
			i++;
			if(i < 700){
				TM1640_display(6,rsec/10); //秒
				TM1640_display(7,rsec%10);
			}else{
				TM1640_display(6,20);	//分
				TM1640_display(7,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,rhour/10);//时	
			TM1640_display(1,rhour%10);
			TM1640_display(2,23); 
			TM1640_display(3,rmin/10);
			TM1640_display(4,rmin%10);	//分
			TM1640_display(5,23);
			
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rsec++;
						if (rsec>59) rsec=0;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(6,rsec/10); //秒
						TM1640_display(7,rsec%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					rsec++;
					if (rsec>59) rsec=0;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if (rsec==0) rsec=60;				
						rsec--;
						delay_ms(KEY_SPEED2);//刷新数值周期
						TM1640_display(6,rsec/10); //秒
						TM1640_display(7,rsec%10);
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					if (rsec==0) rsec=60;				
					rsec--;
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 4;//返回调整日期菜单
				tup=1;//时间更新标志位
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 1;//返回时间显示
				tup=1;//时间更新标志位
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		
		
		// 设置AL0~AL9 十个闹钟
		if(9<MENU && MENU<20){
			// AL0~AL9 10~19
			i++;//配置闪烁效果
			//小时闪烁
			if (j == 2*(MENU-10)&&altime[j]!=24){
				if(i < 700){
					TM1640_display(3,altime[j]/10);//时	
					TM1640_display(4,altime[j]%10);
				}else{
					TM1640_display(3,20);	//分
					TM1640_display(4,20);
				}
				if (i > 1400) i = 0; 
				TM1640_display(5,23);
				TM1640_display(6,altime[j+1]/10);//分
				TM1640_display(7,altime[j+1]%10);	
			}
			//OFF 闪烁 计数到24为关
			if(altime[j]==24){
				if(i < 700){
					TM1640_display(3,30);//O	
					TM1640_display(4,31);//F
					TM1640_display(5,31);//F
					TM1640_display(6,20);
					TM1640_display(7,20);	
				}else{
					TM1640_display(3,20);//O	
					TM1640_display(4,20);//F
					TM1640_display(5,20);//F
					TM1640_display(6,20);
					TM1640_display(7,20);
				}
				if (i > 1400) i = 0; 
			}
			//分钟闪烁
			if(j == 2*(MENU-10)+1){
				if(i < 700){
					TM1640_display(6,altime[j]/10);
					TM1640_display(7,altime[j]%10);	//分
				}else{
					TM1640_display(6,20);	//分
					TM1640_display(7,20);
				}
				if (i > 1400) i = 0; 
				TM1640_display(3,altime[j-1]/10);//时	
				TM1640_display(4,altime[j-1]%10);
				TM1640_display(5,23);
			}
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){ 
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;//长按计数
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						altime[j]++;
						if(j==2*(MENU-10)){
							if (altime[j]>24) altime[j]=0;//如果计数大于24，显示为0
							if(altime[j]!=24){//如果小时计数不是24就正常显示
								TM1640_display(3,altime[j]/10);//时	
								TM1640_display(4,altime[j]%10);
							}
						}else{
							if(altime[j]>59) altime[j]=0;
							TM1640_display(6,altime[j]/10);
							TM1640_display(7,altime[j]%10);	//分
						}
						delay_ms(KEY_SPEED2);//刷新数值周期
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					altime[j]++;
					if(j==2*(MENU-10)){
						if(altime[j]>24) altime[j]=0;
					}else{
						if(altime[j]>59) altime[j]=0;
					}
					BUZZER_BEEP1();//按键提示音
				}
			}
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;//长按计数
					delay_ms(10);
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){//长按处理
						if(j==2*(MENU-10)){
							if (altime[j]==0) altime[j]=25;
							altime[j]--;
							if(altime[j]!=24){//如果小时计数不是24就正常显示
								TM1640_display(3,altime[j]/10);//时	
								TM1640_display(4,altime[j]%10);
							}
						}else{
							if(altime[j]==0) altime[j]=60;
							altime[j]--;
							TM1640_display(6,altime[j]/10);//分
							TM1640_display(7,altime[j]%10);	
						}
						delay_ms(KEY_SPEED2);//刷新数值周期
						BUZZER_BEEP1();//按键提示音
					}
				}else{
					if(j==2*(MENU-10)){
						if (altime[j]==0) altime[j]=25;
						altime[j]--;
					}else{
						if(altime[j]==0) altime[j]=60;
						altime[j]--;
					}
					BUZZER_BEEP1();//按键提示音
				}
			}
			
			//调整定时下一位
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)){
				if(j==2*(MENU-10)&&altime[j]!=24){//判断是否为小时还是分钟，如果是分钟就切换到小时反之亦然
					j=2*(MENU-10)+1;
				}else{
					j=2*(MENU-10);
				}
				ALFLASH_W(altime);//写入flash中
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			
			//确认定时
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)){
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				j=0;//复位
				KEY_VALUE=1;
				ALFLASH_W(altime);//写入flash中
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//闹钟到时处理
		if (MENU == 20){
			//配置闪烁效果
			i++;
			if(i > 1){//因为有蜂鸣器鸣响占用时间，所以只需要i不需要过长计数就可以实现闪烁效果
				TM1640_display(0,28);//A
				TM1640_display(1,29);//L
				TM1640_display(2,e); //触发的闹钟编号
				TM1640_display(3,rhour/10);//到时时间
				TM1640_display(4,rhour%10);
				TM1640_display(5,23);//:
				TM1640_display(6,rmin/10);
				TM1640_display(7,rmin%10);
			}else{
				TM1640_display(0,20);
				TM1640_display(1,20);
				TM1640_display(2,20);
				TM1640_display(3,20);
				TM1640_display(4,20);
				TM1640_display(5,20);
				TM1640_display(6,20);
				TM1640_display(7,20);
			}
			if (i > 3) i = 0;
			BUZZER_BEEP4();//闹钟响铃
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
				KEY_VALUE=1;//返回时间显示菜单
				d=0;//闹钟到时标志位复位
				k=0;//闹钟到时响铃时间计数归零
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
				KEY_VALUE=1;//返回时间显示菜单
				d=0;//闹钟到时标志位复位
				k=0;//闹钟到时响铃时间计数归零
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)){
				KEY_VALUE=1;//返回时间显示菜单
				d=0;//闹钟到时标志位复位
				k=0;//闹钟到时响铃时间计数归零
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)){
				KEY_VALUE=1;//返回时间显示菜单
				d=0;//闹钟到时标志位复位
				k=0;//闹钟到时响铃时间计数归零
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		
		//调整保存的两组温度值
		if (MENU==21 || MENU==22){
			i++;//闪烁计数
			TM1640_display(0,32);//t
			TM1640_display(1,22);//c
			TM1640_display(2,MENU-21); //触发的温度值编号
			TM1640_display(3,20);
			TM1640_display(7,22);
			//配置闪烁效果
			if(j==2*(MENU-21)){//调整整数位
				if(i < 700){
					TM1640_display(4,temp[j]%100/10);//整数位
					TM1640_display(5,temp[j]%10+10);//整数位
				}else{
					TM1640_display(4,20);//数码管灭
					TM1640_display(5,20);
				}
				if (i > 1400) i = 0;
				TM1640_display(6,temp[j+1]%10);//小数位常亮
			}
			if(j==2*(MENU-21)+1){//调整小数位
				if(i < 700){
					TM1640_display(6,temp[j]%10);
				}else{
					TM1640_display(6,20);//数码管灭
				}
				if (i > 1400) i = 0;
				TM1640_display(4,temp[j-1]%100/10);//整数位常亮
				TM1640_display(5,temp[j-1]%10+10);
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						temp[j]++;//累加
						if(j==2*(MENU-21)){//调整整数位
							if(temp[j]>55) temp[j]=0;
							TM1640_display(4,temp[j]%100/10);//整数位
							TM1640_display(5,temp[j]%10+10);//整数位
						} 
						if(j==2*(MENU-21)+1){//调整小数位
							if(temp[j]>9) temp[j]=0;
							TM1640_display(6,temp[j]%10);//保证调整数值时，小数位常亮
						}
						delay_ms(KEY_SPEED2);//刷新数值周期
						BUZZER_BEEP1();//按键提示音
					}
				}else{//单击
					temp[j]++;//累加
					if(j==2*(MENU-21)){//调整整数位
						if(temp[j]>55) temp[j]=0;
					} 
					if(j==2*(MENU-21)+1){//调整小数位
						if(temp[j]>9) temp[j]=0;
					}
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
				i=0;//保证调整时间时，数码管不再闪烁，干扰调时
				//判断是否长按
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //长按计时
				}
				//长按
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if(j==2*(MENU-21)){//调整整数位
							if(temp[j]==0) temp[j]=56;
							TM1640_display(4,temp[j]%100/10);//整数位
							TM1640_display(5,temp[j]%10+10);//整数位
						} 
						if(j==2*(MENU-21)+1){//调整小数位
							if(temp[j]==0) temp[j]=10;
							TM1640_display(6,temp[j]%10);//保证调整数值时，小数位常亮
						}
						temp[j]--;//累减
						delay_ms(KEY_SPEED2);//刷新数值周期
						BUZZER_BEEP1();//按键提示音
					}
				}else{//单击
					if(j==2*(MENU-21)){//调整整数位
						if(temp[j]==0) temp[j]=56;
					} 
					if(j==2*(MENU-21)+1){//调整小数位
						if(temp[j]==0) temp[j]=10;
					}
					temp[j]--;//累减
					BUZZER_BEEP1();//按键提示音
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)){//调整下一位
				if(j==2*(MENU-21)){//判断是否为小时还是分钟，如果是分钟就切换到小时反之亦然
					j=2*(MENU-21)+1;
				}else{
					j=2*(MENU-21);
				}
				TMFLASH_W(temp);//写入flash中
				BUZZER_BEEP1();//按键提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)){
				i=0;//保证跳转回去，数码管常亮
				j=0;//复位
				KEY_VALUE=2;//跳转到温度值
				TMFLASH_W(temp);//写入flash中
				BUZZER_BEEP4();//退出调时提示音
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		if (MENU == 120){
		
		}
		
		if (MENU == 130){
		
		}
	}
	
	
}
