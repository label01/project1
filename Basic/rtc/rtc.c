








#include "rtc.h"
#include "usart.h"

//以下2条全局变量--用于RTC时间的读取
u16 ryear; //4位年
u8 rmon,rday,rhour,rmin,rsec,rweek;//2位月日时分秒周



void RTC_First_Config(void){ //首次启用RTC的设置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); //启用PWR和BKP的时钟
	PWR_BackupAccessCmd(ENABLE);
	BKP_DeInit();
	RCC_LSEConfig(RCC_LSE_ON); //外部32.768Khz晶振开启·
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); // 检查RCC的标志位是否设置RCC_FLAG_LSERDY是LSE低速外部时钟源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); // RTC时钟配置成低速外部时钟源LSE
	RCC_RTCCLKCmd(ENABLE); //开启RTC
	RTC_WaitForSynchro(); //开启后需要等待APB1时钟与RTC时钟同步， 才能读写寄存器
	RTC_WaitForLastTask(); //读写寄存器前，要确定上一个操作已经结束
	RTC_SetPrescaler(32767); //设置RTC分频器，使RTC时钟为1Hz
	RTC_WaitForLastTask();// 等待寄存器写入完成
}



/*实时时钟初始化*/
void RTC_Config(void){
	if(BKP_ReadBackupRegister(BKP_DR1) != 0xa5a5){
		RTC_First_Config(); //重新配置rtc
		BKP_WriteBackupRegister(BKP_DR1, 0xa5a5);
	}else{
		/*后备寄存器没有掉电， 无需重新配置RTC， 开始判断本次复位类型*/
		if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET){
			//上电复位
		}
		else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET){
			//这是上电复位
		}
		RCC_ClearFlag();
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
	}

}

/*
* 判断是否是闰年函数
* 月份 1		2		3		4		5		6		7		8		9		10		11		12
* 闰年 31		29	31	30	31	30	31	31	30	31		30		31
*非闰年31		28	31	30	31	30	31	31	30	31		30		31
*输入:年份
*输出:该年份是不是闰年， 1，是. 0， 不是	
*/
u8 Is_Leap_Year(u16 year){
	if(year%4==0){
		if(year%100==0){
			if(year%400==0)return 1;
			else return 0;
		}else return 1;
	}else return 0;
}



u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表




/*
*按年月日计算星期（只允许1970-2099）
*/
u8 RTC_Get_Week(u16 year,u8 month,u8 day){
	if(month<1||month>12){
		//月份无效，返回错误码，例如255
		return 255;
	}
	u16 temp2;
	u8 yearH, yearL;
	yearH = year/100;
	yearL = year%100;
	if(yearH>19)yearL+=100;
	temp2 = yearL+yearL/4;
	temp2 = temp2%7;
	temp2= temp2+day+table_week[month-1];
	if(yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}


/*
*蔡勒公式，计算星期
*/
u8 RTC_Zeller_Week(u16 year, u8 month, u8 day){
	if(month<1||month>12){
		//月份无效，返回错误码，例如255
		return 255;
	}
	if(month==1||month==2) month+=12, --year;
	return ((day+2*month+3*(month+1)/5+year+year/4-year/100+year/400+1)%7+7)%7;
}





const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};//平年的月份日期表

/**
  *读出时间
  *返回值：0， 成功
	*其他：错误代码
**/
u8 RTC_Get(void){
	static u16 daycnt=0;
	u32 timecount=0;
	u32 temp=0;
	u16 temp1=0;
	timecount=RTC_GetCounter();
	temp=timecount/86400; //得到天数（秒钟数对应）
	if(daycnt!=temp){
		daycnt=temp;
		temp1=1970;
		while(temp>=365){
			if(Is_Leap_Year(temp1)){//是闰年
				if(temp>=366)temp-=366;//闰年的秒数
				else{temp1++;break;}
			}
			else temp-=365;
			temp1++;
		}
		ryear=temp1;
		temp1=0;
		while(temp>=28){//超过一个月
			if(Is_Leap_Year(ryear)&&temp1==1){
				if(temp>=29)temp-=29;
				else break;
			}else{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];
				else break;
			}
			temp1++;
		}
		rmon=temp1+1;
		rday=temp+1;
	}
	temp=timecount%86400;
	rhour=temp/3600;
	rmin=(temp%3600)/60;
	rsec=(temp%3600)%60;
	//rweek=RTC_Get_Week(ryear, rmon, rday);
	rweek=RTC_Zeller_Week(ryear, rmon, rday);
	return 0;

}


/*
*写入时间
*
*/
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec){
	u16 t;
	u32 seccount=0;
	if(syear<2000||syear>2099)return 1;
	for(t=1970;t<syear;t++){
		if(Is_Leap_Year(t))seccount+=31622400;
		else seccount+=31536000;
	}
	smon-=1;
	for(t=0;t<smon;t++){
		seccount+=(u32)mon_table[t]*86400;
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;
	}
	seccount+=(u32)(sday-1)*86400;
	seccount+=(u32)hour*3600;
	seccount+=(u32)min*60;
	seccount+=sec;
	RTC_First_Config();
	BKP_WriteBackupRegister(BKP_DR1, 0xa5a5);
	RTC_SetCounter(seccount);
	RTC_WaitForLastTask();
	return 0;
}









