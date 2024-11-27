/*
��Ʒ�� �������� DoYoung Studio
��������
��д�ˣ� ����
��дʱ�䣺 201 ������
Ӳ��֧�֣� STM32F103C8 �ⲿ���� 8M
�޸���־��
1-
*/
#include "stm32f10x.h" //STM ͷ�ļ�

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

#define KEY_SPEED1	100		//������ʱ
#define KEY_SPEED2 100		//��ֵˢ�¼��ms


int main(void) {//������
	
	
	u8 buffer[3]; //�����¶�ֵ
	u8 MENU=0; //�˵�ֵ
	u8 MENU2=0; //�����˵�ֵ
	u16 i=0; // ����i
	u8 j=0; //����j
	u16 k=0; //����k�� ���ڿ�����������ʱ��
	u8 a=0; //��������
	u16 b=0; //ѭ����ʾ���� ʱ�� �¶�
	u16 c=0;//�ݴ�����ֵ
	u8 d=0; //���ӵ�ʱ��־λ
	u8 e=0; //���ӵ�ʱ���Ӻ�
	u8 tup=0;//����ʱ���־λ
	u8 altime[20]={0};//�����Ǹ����ӵ�ʱ ��
	u8 temp[4]={0}; //�����¶ȵ�������С��
	RCC_Configuration();//ϵͳʱ�ӳ�ʼ��
	LED_Init();//LED��ʼ��
	TOUCH_KEY_Init();//����������ʼ��
	
	RTC_Config();// RTC��ʼ��
	TM1640_Init(); //TM1640��ʼ��
	I2C_Configuration(); //i2c��ʼ��
	TOUCH_KEY_Init(); //����������ʼ��
	BUZZER_Init();//��������ʼ��
	delay_ms(1000);//��ʱ1S�ȴ�
	BUZZER_BEEP2();//������ʾ��
	
	//��FLASH����
	for(i=0; i<12; i++){//����ʮ�����ӷ�����ֵ+2���¶�ֵ
		c=FLASH_R(FLASH_START_ADDR+i*2);//��ָ��ҳ�ĵ�ַ��FLASH
		if(i<10){
			altime[2*i]=c/0x100;
			altime[2*i+1]=c%0x100;
		}else{//�¶�ֵ��ֵ
			temp[(i-10)*2]=c/0x100;
			temp[(i-10)*2+1]=c%0x100;
		}
	}
	
	//�жϣ��������0��Сʱֵ����24�ͳ�ʼ�����ӵ���������
	if(altime[0]>24){
		//��ʼ������ֵ
		for(i=0;i<10;i++){
			altime[2*i]=24;
			altime[2*i+1]=0;
		}
	}
	
	//�жϣ�����¶�0��Сʱֵ����55�ͳ�ʼ���¶ȵ���������
	if(temp[0]>55){
		//��ʼ������ֵ
		for(i=0;i<2;i++){
			temp[2*i]=0;
			temp[2*i+1]=0;
		}
	}
	
	//д��һ�����ӵ��������ݵ�FLASH��
	ALFLASH_W(altime);
	
	//д��һ�����õ��¶�ֵ
	TMFLASH_W(temp);
	
	while (1) {
		//�˵�ÿ��ѭ�������»��һ���ֵ
		MENU = KEY_VALUE;
		//tup��ʱ����±�־���ж��Ƿ�����ʱ��
		if (tup == 1){
			tup=0;//ʱ����±�־��λ
			RTC_Set(ryear, rmon, rday, rhour, rmin, rsec);//����ʱ��
		}
		//��ȡʱ��ͻ�����Ϣ���ض��˵���
		if(MENU < 3 || MENU > 20){
			//��ȡRTCʱ��
			RTC_Get();
			//��ȡ�����¶�ֵ
			LM75A_GetTemp(buffer);
		}
		//����ʱ���¶�ѭ����ʾ��ʱ
		b++;
		if(MENU<3){
			if(b==1) KEY_VALUE=1;
			if(b==3000) KEY_VALUE=0;
			if(b==5000) KEY_VALUE=2;
			if(b==7000) b=0;
			//menu�˵�ֵ����3ʱb��ֵ���ܻ����7000
			if(b>7000) b=0;
		}
		
		//���ӵ�ʱ���
		if(MENU<3 && d==0){//ʱ��������ʾ������δ����
			for(i=0;i<10; i++){//ѭ��������������Ƿ�ʱ
				if(altime[2*i]==rhour&&altime[2*i+1]==rmin &&rsec==0){
					d=1;//���ӵ�ʱ��־λ��1����
					e=i;//���浽ʱ���ӱ��
					KEY_VALUE=20;//���ӵ�ʱ����
				}
			}
		}
		
		
		if(d==1){//���ӵ�ʱ������ʱ�����
			k++;//��ʱ��������������ʱ��
			if(k>1000){
				KEY_VALUE=1;//����ʱ�䳬ʱ������ʱ��˵�
				d=0;//���ӵ�ʱ��־λ��λ
				k=0;//��ʱ��������
			}
		}
		
		
		//����ѭ������
		//������
		if (MENU==0){
			TM1640_display(0,ryear%100/10);	//��
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21); 
			TM1640_display(3,rmon/10);	//��
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21);
			TM1640_display(6,rday/10); //��
			TM1640_display(7,rday%10);
			Read_Key_Value();
			
			
		}
		//ʱ����
		if (MENU==1){
			TM1640_display(0,rhour/10);//ʱ	
			TM1640_display(1,rhour%10);
			TM1640_display(2,23); 
			TM1640_display(3,rmin/10);
			TM1640_display(4,rmin%10);	//��
			TM1640_display(5,23);
			TM1640_display(6,rsec/10); //��
			TM1640_display(7,rsec%10);
			Read_Key_Value();
		}
		//��ʾ�¶�ֵ
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
		//���ò˵�
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
				//�����˵��������
				MENU2++;
				if (MENU2>12) MENU2=0;
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				//�����˵�AL0~AL9 һ������10������ + 2���¶�ֵ �����˵��������
				if (MENU2==0) MENU2=13;
				MENU2--;
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE=0;//ȡ������������ʾ
				MENU2=0; //�����˵���λ
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
				
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				//ȷ����ת�˵�
				if(MENU2 == 0){
					KEY_VALUE=4;
				}
				if(MENU2>0 && MENU2<11){
					KEY_VALUE=9+MENU2;
					j=2*(KEY_VALUE-10);//j������������
				}
				else{
					KEY_VALUE=10+MENU2;
					j=2*(KEY_VALUE-21);//j���¶�ֵ������
				}
				MENU2=0; //�����˵���λ
				BUZZER_BEEP3();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//������
		if (MENU == 4){
			//������˸Ч��
			i++;
			if(i < 700){
				TM1640_display(0,ryear%100/10);	//��
				TM1640_display(1,ryear%100%10);
			}else{
				TM1640_display(0,20);	//��
				TM1640_display(1,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(2,21); 
			TM1640_display(3,rmon/10);	//��
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21);
			TM1640_display(6,rday/10); //��
			TM1640_display(7,rday%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) {
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						ryear++;
						if (ryear>2099) ryear=1970;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(0,ryear%100/10);	//��
						TM1640_display(1,ryear%100%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{//����
					ryear++;
					if (ryear>2099) ryear=1970;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ	
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){//�ж��Ƿ񳤰�
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						ryear--;
						if (ryear<1970) ryear=1970;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(0,ryear%100/10);	//�� 
						TM1640_display(1,ryear%100%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					ryear--;
					if (ryear<1970) ryear=1970;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 5;//��ת����һ���˵�
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 0;//���س�ʼ�˵�
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//������
		if (MENU == 5){
			//������˸Ч��
			i++;
			if(i < 700){
				TM1640_display(3,rmon/10);	//��
				TM1640_display(4,rmon%10);
			}else{
				TM1640_display(3,20);	//��
				TM1640_display(4,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,ryear%100/10);	//��
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21); 
			TM1640_display(5,21);
			TM1640_display(6,rday/10); //��
			TM1640_display(7,rday%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rmon++;
						if (rmon>12) rmon=1;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(3,rmon/10);	//��
						TM1640_display(4,rmon%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					rmon++;
					if (rmon>12) rmon=1;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						rmon--;
						if (rmon<1) rmon=12;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(3,rmon/10);	//��
						TM1640_display(4,rmon%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					rmon--;
					if (rmon<1) rmon=12;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 6;
				//ʱ����±�־λ
				tup=1;//ʱ����±�־λ��1����������ʱ��
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 0;//���س�ʼ�˵�
				tup=1;
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		
		//������
		if (MENU == 6){
			//������˸Ч��
			i++;
			if(i < 700){
				TM1640_display(6,rday/10); //��
				TM1640_display(7,rday%10);
			}else{
				TM1640_display(6,20); //��
				TM1640_display(7,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,ryear%100/10);	//��
			TM1640_display(1,ryear%100%10);
			TM1640_display(2,21); 
			TM1640_display(3,rmon/10);	//��
			TM1640_display(4,rmon%10);	
			TM1640_display(5,21);
			
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rday++;
						if (rday>31) rday=1;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(6,rday/10); //��
						TM1640_display(7,rday%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					rday++;
					if (rday>31) rday=1;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						rday--;
						if (rday<1) rday=31;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(6,rday/10); //��
						TM1640_display(7,rday%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					rday--;
					if (rday<1) rday=31;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 7;//��ת��һ���˵�
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 0;
				//ʱ����±�־λ
				tup=1;
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//����ʱ
		if (MENU == 7){
			//������˸Ч��
			i++;
			if(i < 700){
				TM1640_display(0,rhour/10);//ʱ	
				TM1640_display(1,rhour%10);
			}else{
				TM1640_display(0,20);	//ʱ
				TM1640_display(1,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(2,23); 
			TM1640_display(3,rmin/10);
			TM1640_display(4,rmin%10);	//��
			TM1640_display(5,23);
			TM1640_display(6,rsec/10); //��
			TM1640_display(7,rsec%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//�����ۼ�
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rhour++;
						if (rhour>23) rhour=0;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(0,rhour/10);//ʱ	
						TM1640_display(1,rhour%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					rhour++;
					if (rhour>23) rhour=0;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//�����ۼ�
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if (rhour==0) rhour=24;	
						rhour--;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(0,rhour/10);//ʱ	
						TM1640_display(1,rhour%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					if (rhour==0) rhour=24;	
					rhour--;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 8;//��ת��һ���˵�
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 1;//��ת��ʱ��˵�
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//���÷���
		if (MENU == 8){
			//������˸Ч��
			i++;
			if(i < 700){
				TM1640_display(3,rmin/10);
				TM1640_display(4,rmin%10);	//��
			}else{
				TM1640_display(3,20);	//��
				TM1640_display(4,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,rhour/10);//ʱ	
			TM1640_display(1,rhour%10);
			TM1640_display(2,23); 
			TM1640_display(5,23);
			TM1640_display(6,rsec/10); //��
			TM1640_display(7,rsec%10);
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rmin++;
						if (rmin>59) rmin=0;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(3,rmin/10);
						TM1640_display(4,rmin%10);	//��
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					rmin++;
					if (rmin>59) rmin=0;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//�����ۼ�
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if (rmin==0) rmin=60;				
						rmin--;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(3,rmin/10);
						TM1640_display(4,rmin%10);	//��
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					if (rmin==0) rmin=60;				
					rmin--;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 9;//��ת��һ���˵�
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 1;//����ʱ����ʾ
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//��������
		if (MENU == 9){
			//������˸Ч��
			i++;
			if(i < 700){
				TM1640_display(6,rsec/10); //��
				TM1640_display(7,rsec%10);
			}else{
				TM1640_display(6,20);	//��
				TM1640_display(7,20);
			}
			if (i > 1400) i = 0;
			TM1640_display(0,rhour/10);//ʱ	
			TM1640_display(1,rhour%10);
			TM1640_display(2,23); 
			TM1640_display(3,rmin/10);
			TM1640_display(4,rmin%10);	//��
			TM1640_display(5,23);
			
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) { 
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						rsec++;
						if (rsec>59) rsec=0;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(6,rsec/10); //��
						TM1640_display(7,rsec%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					rsec++;
					if (rsec>59) rsec=0;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) {
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if (rsec==0) rsec=60;				
						rsec--;
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						TM1640_display(6,rsec/10); //��
						TM1640_display(7,rsec%10);
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					if (rsec==0) rsec=60;				
					rsec--;
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) {
				KEY_VALUE = 4;//���ص������ڲ˵�
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) {
				KEY_VALUE = 1;//����ʱ����ʾ
				tup=1;//ʱ����±�־λ
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		
		
		// ����AL0~AL9 ʮ������
		if(9<MENU && MENU<20){
			// AL0~AL9 10~19
			i++;//������˸Ч��
			//Сʱ��˸
			if (j == 2*(MENU-10)&&altime[j]!=24){
				if(i < 700){
					TM1640_display(3,altime[j]/10);//ʱ	
					TM1640_display(4,altime[j]%10);
				}else{
					TM1640_display(3,20);	//��
					TM1640_display(4,20);
				}
				if (i > 1400) i = 0; 
				TM1640_display(5,23);
				TM1640_display(6,altime[j+1]/10);//��
				TM1640_display(7,altime[j+1]%10);	
			}
			//OFF ��˸ ������24Ϊ��
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
			//������˸
			if(j == 2*(MENU-10)+1){
				if(i < 700){
					TM1640_display(6,altime[j]/10);
					TM1640_display(7,altime[j]%10);	//��
				}else{
					TM1640_display(6,20);	//��
					TM1640_display(7,20);
				}
				if (i > 1400) i = 0; 
				TM1640_display(3,altime[j-1]/10);//ʱ	
				TM1640_display(4,altime[j-1]%10);
				TM1640_display(5,23);
			}
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){ 
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;//��������
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						altime[j]++;
						if(j==2*(MENU-10)){
							if (altime[j]>24) altime[j]=0;//�����������24����ʾΪ0
							if(altime[j]!=24){//���Сʱ��������24��������ʾ
								TM1640_display(3,altime[j]/10);//ʱ	
								TM1640_display(4,altime[j]%10);
							}
						}else{
							if(altime[j]>59) altime[j]=0;
							TM1640_display(6,altime[j]/10);
							TM1640_display(7,altime[j]%10);	//��
						}
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					altime[j]++;
					if(j==2*(MENU-10)){
						if(altime[j]>24) altime[j]=0;
					}else{
						if(altime[j]>59) altime[j]=0;
					}
					BUZZER_BEEP1();//������ʾ��
				}
			}
			
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;//��������
					delay_ms(10);
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){//��������
						if(j==2*(MENU-10)){
							if (altime[j]==0) altime[j]=25;
							altime[j]--;
							if(altime[j]!=24){//���Сʱ��������24��������ʾ
								TM1640_display(3,altime[j]/10);//ʱ	
								TM1640_display(4,altime[j]%10);
							}
						}else{
							if(altime[j]==0) altime[j]=60;
							altime[j]--;
							TM1640_display(6,altime[j]/10);//��
							TM1640_display(7,altime[j]%10);	
						}
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						BUZZER_BEEP1();//������ʾ��
					}
				}else{
					if(j==2*(MENU-10)){
						if (altime[j]==0) altime[j]=25;
						altime[j]--;
					}else{
						if(altime[j]==0) altime[j]=60;
						altime[j]--;
					}
					BUZZER_BEEP1();//������ʾ��
				}
			}
			
			//������ʱ��һλ
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)){
				if(j==2*(MENU-10)&&altime[j]!=24){//�ж��Ƿ�ΪСʱ���Ƿ��ӣ�����Ƿ��Ӿ��л���Сʱ��֮��Ȼ
					j=2*(MENU-10)+1;
				}else{
					j=2*(MENU-10);
				}
				ALFLASH_W(altime);//д��flash��
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			
			//ȷ�϶�ʱ
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)){
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				j=0;//��λ
				KEY_VALUE=1;
				ALFLASH_W(altime);//д��flash��
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		//���ӵ�ʱ����
		if (MENU == 20){
			//������˸Ч��
			i++;
			if(i > 1){//��Ϊ�з���������ռ��ʱ�䣬����ֻ��Ҫi����Ҫ���������Ϳ���ʵ����˸Ч��
				TM1640_display(0,28);//A
				TM1640_display(1,29);//L
				TM1640_display(2,e); //���������ӱ��
				TM1640_display(3,rhour/10);//��ʱʱ��
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
			BUZZER_BEEP4();//��������
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
				KEY_VALUE=1;//����ʱ����ʾ�˵�
				d=0;//���ӵ�ʱ��־λ��λ
				k=0;//���ӵ�ʱ����ʱ���������
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
				KEY_VALUE=1;//����ʱ����ʾ�˵�
				d=0;//���ӵ�ʱ��־λ��λ
				k=0;//���ӵ�ʱ����ʱ���������
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)){
				KEY_VALUE=1;//����ʱ����ʾ�˵�
				d=0;//���ӵ�ʱ��־λ��λ
				k=0;//���ӵ�ʱ����ʱ���������
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)){
				KEY_VALUE=1;//����ʱ����ʾ�˵�
				d=0;//���ӵ�ʱ��־λ��λ
				k=0;//���ӵ�ʱ����ʱ���������
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		
		//��������������¶�ֵ
		if (MENU==21 || MENU==22){
			i++;//��˸����
			TM1640_display(0,32);//t
			TM1640_display(1,22);//c
			TM1640_display(2,MENU-21); //�������¶�ֵ���
			TM1640_display(3,20);
			TM1640_display(7,22);
			//������˸Ч��
			if(j==2*(MENU-21)){//��������λ
				if(i < 700){
					TM1640_display(4,temp[j]%100/10);//����λ
					TM1640_display(5,temp[j]%10+10);//����λ
				}else{
					TM1640_display(4,20);//�������
					TM1640_display(5,20);
				}
				if (i > 1400) i = 0;
				TM1640_display(6,temp[j+1]%10);//С��λ����
			}
			if(j==2*(MENU-21)+1){//����С��λ
				if(i < 700){
					TM1640_display(6,temp[j]%10);
				}else{
					TM1640_display(6,20);//�������
				}
				if (i > 1400) i = 0;
				TM1640_display(4,temp[j-1]%100/10);//����λ����
				TM1640_display(5,temp[j-1]%10+10);
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)){
						temp[j]++;//�ۼ�
						if(j==2*(MENU-21)){//��������λ
							if(temp[j]>55) temp[j]=0;
							TM1640_display(4,temp[j]%100/10);//����λ
							TM1640_display(5,temp[j]%10+10);//����λ
						} 
						if(j==2*(MENU-21)+1){//����С��λ
							if(temp[j]>9) temp[j]=0;
							TM1640_display(6,temp[j]%10);//��֤������ֵʱ��С��λ����
						}
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						BUZZER_BEEP1();//������ʾ��
					}
				}else{//����
					temp[j]++;//�ۼ�
					if(j==2*(MENU-21)){//��������λ
						if(temp[j]>55) temp[j]=0;
					} 
					if(j==2*(MENU-21)+1){//����С��λ
						if(temp[j]>9) temp[j]=0;
					}
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
				i=0;//��֤����ʱ��ʱ������ܲ�����˸�����ŵ�ʱ
				//�ж��Ƿ񳤰�
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) && a < KEY_SPEED1){
					a++;
					delay_ms(10); //������ʱ
				}
				//����
				if(a>=KEY_SPEED1){
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)){
						if(j==2*(MENU-21)){//��������λ
							if(temp[j]==0) temp[j]=56;
							TM1640_display(4,temp[j]%100/10);//����λ
							TM1640_display(5,temp[j]%10+10);//����λ
						} 
						if(j==2*(MENU-21)+1){//����С��λ
							if(temp[j]==0) temp[j]=10;
							TM1640_display(6,temp[j]%10);//��֤������ֵʱ��С��λ����
						}
						temp[j]--;//�ۼ�
						delay_ms(KEY_SPEED2);//ˢ����ֵ����
						BUZZER_BEEP1();//������ʾ��
					}
				}else{//����
					if(j==2*(MENU-21)){//��������λ
						if(temp[j]==0) temp[j]=56;
					} 
					if(j==2*(MENU-21)+1){//����С��λ
						if(temp[j]==0) temp[j]=10;
					}
					temp[j]--;//�ۼ�
					BUZZER_BEEP1();//������ʾ��
				}
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)){//������һλ
				if(j==2*(MENU-21)){//�ж��Ƿ�ΪСʱ���Ƿ��ӣ�����Ƿ��Ӿ��л���Сʱ��֮��Ȼ
					j=2*(MENU-21)+1;
				}else{
					j=2*(MENU-21);
				}
				TMFLASH_W(temp);//д��flash��
				BUZZER_BEEP1();//������ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
			}
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)){
				i=0;//��֤��ת��ȥ������ܳ���
				j=0;//��λ
				KEY_VALUE=2;//��ת���¶�ֵ
				TMFLASH_W(temp);//д��flash��
				BUZZER_BEEP4();//�˳���ʱ��ʾ��
				while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
			}
		}
		
		if (MENU == 120){
		
		}
		
		if (MENU == 130){
		
		}
	}
	
	
}
