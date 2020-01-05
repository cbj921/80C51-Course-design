#include <reg52.h>
#include "delay.h"
#include "buzzer.h"
#include "digitalTube.h"
#include "timer.h"
#include "key.h"
#include "led.h"
#include "uart.h"
// ע�����ʹ�ô���ʱһ��Ҫ��ѡ ���������С� ���������������Ҫ�� �����ڡ� ɾ����ֱ������ݿ�ͷ
//unsigned int test=0;
// �������ڵı���
unsigned int alarmYear=0;
unsigned char alarmMonth=0,alarmDay=0,alarmHour=0,alarmMinute=0; 
unsigned char alarmFlag = 0; // ���ӱ�־λ 0���ر�  1������

unsigned char monthDay[] = {31,28,31,30,31,30,31,31,30,31,30,31}; // ������ʱ��ƽ����
unsigned char getData;
unsigned char sdat[64]={0x00};        // ���ݻ�����
unsigned char sendDat[16]={0x00};     // �������ݻ�����
unsigned char sendIndex = 0;         // ���ͻ�����ָ��
unsigned char sp=0;   // ���ݻ�����ָ��

unsigned char buzzerKeyFlag = 0;
unsigned char keyFlag=0;
unsigned char countSecond = 0; // ��������3��
unsigned char ledTime = 0;

// ͨ���ж���������������
unsigned char hour=0,minute=0,second=0,month=1,day=21,week = 0;
unsigned int year=2020; 
unsigned char T0Num = 0;
unsigned char modeFlag = 0; // 0����ʾ���� 1����ʾ��Ԫ��  2����ʾ����

void updateTimeData();
void showTime_MS();
void showTime_Year();
void stayThreeSecond();
void showTime_MonthDay();
void showWeekLed();
unsigned char getWeek(unsigned int tYear,unsigned char tMonth,unsigned char tDay);
void dealData(unsigned char *dataGo);
unsigned char getTimeFromData(unsigned char *dataGo);
void clearSDAT();
unsigned char judgeLeapOrOrdinaryYear(unsigned int tYear);
unsigned char getMonthDay(unsigned char tYear,unsigned char tMonth);
unsigned char getSDATLength(unsigned char *point);
void analyChar(unsigned char *point,unsigned char length);
unsigned int getYearFromQuestion(unsigned char *point);
void sendData(unsigned char *point);
unsigned char getMonthFromQuestion(unsigned char *point);
unsigned char getWorkDayFromMonth(unsigned int yearGo,unsigned char monthGo);
void setAlarmClock(unsigned int yearGo,unsigned char monthGo,unsigned char dayGo,unsigned char hourGo,unsigned char minuteGo);
void checkAlarmTime();
void sendAllSDAT();

int main()
{
	// init������ܳ�ʼ��
	timerInit_0();           // ��ʼ����ʱ��0
	uart_init();             // ��ʼ������
	week = getWeek(year,month,day);		 // ��ʼ������
	showWeekLed();           // ��ʾLED
	clearSDAT();             // ��ջ�����
	///////////////////////
	while(1)
	{	
		updateTimeData();   // ����ʱ������
		modeFlag = checkKey_s1(&keyFlag,modeFlag); // ��ⰴ�� S1
		modeFlag = checkKey_s2(&keyFlag,modeFlag); // ��ⰴ�� S2
		closeBuzzer_s3(&buzzerKeyFlag,&alarmFlag);			   // ��ⰴ�� S3�������£���رշ�����
		switch(modeFlag){
			case 0: showTime_MS(); break;
			case 1: showTime_Year(); break;
			case 2: showTime_MonthDay(); break;
		}	
	}
	return 0;
}

// ��ʱ��T0�жϺ���
void T0_func(void) interrupt 1
{
	// T0Ϊ50ms���һ��
	// ��װ������
	TH0 = 0x3c;    					//װ��ʱ��ֵ��8λ
	TL0 = 0xb0;    					//װ��ʱ��ֵ��8λ
	T0Num++;
	if(T0Num == 20)
	{
		T0Num = 0;
		second++;
		showWeekLed();             // ��LED����ʾ��ѭ��
	}
	stayThreeSecond();             // ���������	
}
// ����ʱ������
void updateTimeData()
{
	if(second >=60)
	{
		second = 0;
		minute ++;
		if(alarmFlag)
		{
			checkAlarmTime(); // �������	
		}
	}
	if(minute >= 60)
	{
		minute = 0;
		hour ++;
	}
	if(hour >= 24)
	{
		hour = 0;
		day++;
		week = getWeek(year,month,day);          //��������
	}
	if(day > getMonthDay(year,month))
	{
		day = 1;
		month++;	
	}
	if(month > 12)
	{
		month = 1;
		year++;
	}
}
// ��ʾ����ĺ���
void showTime_MS()
{
	unsigned char m1,m2,s1,s2;
	m2 = minute %10; // ���Ӹ�λ
	m1 = minute /10; // ����ʮλ
	s2 = second %10; // ���λ
	s1 = second /10; //	��ʮλ
	showFourNum(m1,m2,s1,s2,1);
}
// ��ʾ��Ԫ��
void showTime_Year()
{
	unsigned char y1,y2,y3,y4;
	unsigned int tempYear = year;
	y4 = tempYear %10;
	tempYear = tempYear/10;
	y3 = tempYear %10;
	tempYear = tempYear/10;
	y2 = tempYear %10;
	tempYear = tempYear/10;
	y1 = tempYear %10;
	showFourNum(y1,y2,y3,y4,0);
}
// ��ʾ����
void showTime_MonthDay()
{
	unsigned char m1,m2,d1,d2;
	m2 = month %10;
	m1 = month /10;
	d2 = day %10;
	d1 = day /10;
	showFourNum(m1,m2,d1,d2,1);	
}
// ��ʾ����ĵȴ�����
void stayThreeSecond()
{
	if(keyFlag == 1)
	{
		countSecond ++;
		if(countSecond == 60)
		{
			keyFlag = 0;
			modeFlag = 0;
			countSecond = 0;
		}
	}
}
// �������ں���
unsigned char getWeek(unsigned int tYear,unsigned char tMonth,unsigned char tDay)
{
	unsigned char topTwoYear ;              // ���ǰ��λ(Ĭ�������λ��)
	unsigned char lastTwoYear ;             // ��ݺ���λ
	unsigned char dayGo = tDay;				// ��
	unsigned char monthGo = tMonth;		    // 3<=monthGo<=14,��1��2�µ���ǰ���13��14��
	unsigned char weekGo;

	if(monthGo<3)
	{
		if(monthGo == 1) monthGo = 13;
		else if(monthGo == 2) monthGo = 14;
		tYear --; 
	}
	topTwoYear = tYear/100;
	lastTwoYear = tYear%100;
	weekGo = ((topTwoYear/4)-2*topTwoYear+lastTwoYear+(lastTwoYear/4)+((13*(monthGo+1))/5)+dayGo-1)%7;
	return weekGo;
}
// led��ʾ�����ͷ���������
void showWeekLed()
{
	if(week !=0)
	{
		week = getWeek(year,month,day);
		showLed(week,ledTime%8);
		ledTime++;	
	}
	else
	{
		clearAllLed();
		buzzer_reverse();	// ̫������ʱע��
	}
}

// �����жϺ���
void uart_func(void) interrupt 4
{
	unsigned char length;
	// ��������
	if(RI)
	{
		// һ�����ֻ���Ӣ���ַ� ռ�� һ���ֽ�
		// һ������ռ��     �����ֽ�
		RI = 0;
		getData = SBUF;
		sdat[sp] = getData;
		sp++;
		if(sp>=64) sp = 0;
		if(getData == 0x0A)
		{
			// 0x0AΪ������־λ
			// ͨ���ж���������Ч�������ж���У׼���Ǵ���ģʽ
			length = getSDATLength(sdat);
			if(length == 19)
			{
				// �������ݴ�����ֵΪ��Ӧ�ı���
				dealData(sdat);	
			}  
			else
			{
				// �����ַ�������������
				analyChar(sdat,length);
				sendData(sendDat);	
			} 
			// test���������ݻ�����ȫ������
			//sendAllSDAT();
			// ���sdat��������sp����
			clearSDAT();
			sp = 0;
		}
		//SBUF = getData;
		//test
	}
	if(TI)
	{
		//TI =0;
	}
}
// ���������������
void dealData(unsigned char *dataGo)
{
	unsigned char timeFlag = 0;  // ����ʱ��ı�־λ��0���꣬1���£�2���գ��Դ�����
	unsigned char temp=0,i;
	// ��ԭʱ�����ݶ�����
	year = 0,month = 0,day = 0,hour = 0,minute = 0,second = 0;

	while(*dataGo != 0x0D)
	{
		// ������������
		if(*dataGo == 0x2F || *dataGo == 0x3A ||*dataGo == 0x2D) // ��Ϊб�˺�ð�ŵ�ʮ������Ϊ 0x2f��0x3a
		{
			timeFlag++;	
			dataGo++;
		}
		switch(timeFlag){
			case 0:
				for(i=0;i<4;i++)
				{
					temp = *dataGo - 0x30;
					year *= 10;
					year += temp;
					dataGo++; 
				}
				break;
			case 1:
				month = getTimeFromData(dataGo); // ��Ϊ����ֻ�Ǵ�ֵ������ı�������datago��ַ
				dataGo += 2;					 // ��������Ҫ�ں������������� 2 ����
				break;
			case 2:
				day = getTimeFromData(dataGo);
				dataGo += 2;
				break;
			case 3:
				hour = getTimeFromData(dataGo);
				dataGo += 2;
				break;
			case 4:
				minute = getTimeFromData(dataGo);
				dataGo += 2;
				break;
			case 5:
				second = getTimeFromData(dataGo);
				dataGo += 2;
				break;
		}	
	}				
}
// ��������ʱ����ĺ���
unsigned char getTimeFromData(unsigned char *point)
{
	unsigned char temp=0,i;
	unsigned char returnValue=0;
	for(i=0;i<2;i++)
	{
		temp = *point - 0x30;
		returnValue *= 10;
		returnValue += temp;
		point++;
	}
	return returnValue; 
}
// ��ջ�����
void clearSDAT()
{
	unsigned char i;
	for(i=0;i<64;i++)
	{
		sdat[i] = 0x00;
	}
}
// �ж�ƽ����
unsigned char judgeLeapOrOrdinaryYear(unsigned int tYear)
{
	unsigned int tempYear = tYear;
	if((tempYear%4 == 0 && tempYear%100 !=0)||(tempYear%100 == 0 && tempYear%400==0))
	{
		return 1;	
	}
	return 0;
}
// �õ���Ӧ�·��м���
unsigned char getMonthDay(unsigned char tYear,unsigned char tMonth)
{
	if(tMonth == 2)
	{
		if(judgeLeapOrOrdinaryYear(tYear))
		{
			return 29;
		}
	}
	return monthDay[tMonth-1];		
}
// ���SDAT����Ч���ݳ���
unsigned char getSDATLength(unsigned char *point)
{
	unsigned char i=0;
	for(i=0;*point != 0x0D;)
	{
		i++;
		point++;	
	}
	return i;
}
// �ַ�����
void analyChar(unsigned char *point,unsigned char length)
{
	unsigned int tempYear=0;
	unsigned char tempMonth=0,tempDay=0,tempWeek=0;
	unsigned char tempHour=0,tempMinute=0;

	tempYear = getYearFromQuestion(point); // ������
	if(length == 16)
	{
		// �ش��Ƿ�Ϊ����
		if(judgeLeapOrOrdinaryYear(tempYear))
		{
			sendDat[0] = 0xCA;
			sendDat[1] = 0xC7;
		}
		else
		{
			sendDat[0] = 0xB7;
			sendDat[1] = 0xF1;
		}
		sendDat[2] = 0x0D;
		sendDat[3] = 0x0A; // �س�
		sendDat[4] = 0x3A; // 0x3AΪ������־λ
	}
	else if(length == 24)
	{
		// �ش������ڼ�
		tempMonth = getTimeFromData(&point[6]); // ����·�
		tempDay = getTimeFromData(&point[10]); // �����
		tempWeek = getWeek(tempYear,tempMonth,tempDay);
		sendDat[0] = 0xD0;
		sendDat[1] = 0xC7; // ��
		sendDat[2] = 0xC6;
		sendDat[3] = 0xDA; // ��
		sendDat[4] = tempWeek + 0x30;
		sendDat[5] = 0x0D;
		sendDat[6] = 0x0A; // �س�
		sendDat[7] = 0x3A; // 0x3AΪ������־λ		
	}
	else if(length == 22)
	{
		// ��������һ���ʺţ�Ϊ�ش��м���������
		tempMonth = getTimeFromData(&point[6]); // ����·�
		tempDay	  = getWorkDayFromMonth(tempYear,tempMonth); // ������tempDay��Ϊ�����յı���
		sendDat[0] = (tempDay /10) +0x30;
		sendDat[1] = (tempDay %10) +0x30;
		sendDat[2] = 0xCC;
		sendDat[3] = 0xEC; // ���족
		sendDat[4] = 0x0D;
		sendDat[5] = 0x0A; // �س�
		sendDat[6] = 0x3A;  // 0x3AΪ������־λ	
	}
	else if(length == 42) 
	{
		// ����չ4�����ӹ��ܡ�
		// Ҫ�� �����ڡ� ɾ����ֱ�Ӵ���ݿ�ʼ
		tempMonth = getTimeFromData(&point[6]); // ����·�	 
		tempDay = getTimeFromData(&point[10]); // �����
		tempHour = getTimeFromData(&point[14]); // ���Сʱ
		tempMinute = getTimeFromData(&point[18]); // ��÷���
		// ���ö�ʱ����
		setAlarmClock(tempYear,tempMonth,tempDay,tempHour,tempMinute);
		//test
		/*	
		sendDat[0] = (tempMinute /10) +0x30;
		sendDat[1] = (tempMinute %10) +0x30;
		sendDat[2] = 0x3A;  // 0x3AΪ������־λ
		*/	
	}	
}
// �������л�����
unsigned int getYearFromQuestion(unsigned char *point)
{
	// ��Ϊ��ݾ���ǰ4λ
	unsigned char i=0,temp=0;
	unsigned int yearGo=0;
	while(i<4)
	{
		temp = *point - 0x30;
		yearGo *= 10;
		yearGo += temp;
		i++;
		point++;			
	}
	return yearGo;	
}
// ���ͷ��ͻ�����������
void sendData(unsigned char *point)
{
	unsigned char i;
	unsigned char *temp = point;
	// ��Ϊ���ͻ��������������� 0x3A ����Ϊ��ֹ
	while(*point != 0x3A)
	{
		SBUF = *point;
		while(!TI);   // �����ڷ��͵�ʱ��ȴ� ,TI == 1 ��ʾ�������
		TI = 0;
		point++;
	}
	// ������ͻ�����
	for(i=0;i<16;i++)
	{
		temp[i] = 0x00;
	}		
}
// ���·��л�ù������м���
unsigned char getWorkDayFromMonth(unsigned int yearGo,unsigned char monthGo)
{
	// ע����ݶ�Ӧ���� int �ͣ���������
	unsigned char tempWeek=0,tempDay=0;
	char workDay = 0;
	// �Ȼ�ȡ���µ� 1�������ڼ�
	tempWeek = getWeek(yearGo,monthGo,1);
	if(tempWeek == 0) tempWeek = 7;
	// �ٻ�ȡ����һ���м���
	tempDay =  getMonthDay(yearGo,monthGo);
	// ���㹤����
	workDay += (6-tempWeek);
	if(workDay == -1) workDay = 0;
	// ����ʣ�������
	tempDay -= (8 - tempWeek);
	// ����ʣ�������еĹ�����
	workDay += (tempDay/7)*5; 
	tempDay = tempDay %7;
	if(tempDay >5) tempDay = 5;
	workDay += tempDay;

	return workDay;
}
// ���ö�ʱ����
void setAlarmClock(unsigned int yearGo,unsigned char monthGo,unsigned char dayGo,unsigned char hourGo,unsigned char minuteGo)
{
	alarmYear = yearGo;
	alarmMonth = monthGo;
	alarmDay = dayGo;
	alarmHour = hourGo;
	alarmMinute = minuteGo;
	alarmFlag = 1; // ��������
	// ͨ�����ڻ�Ӧ���������ú���
	// �ش� it's-ok
	sendDat[0] = 0x49; 
	sendDat[1] = 0x74;
	sendDat[2] = 0x60;
	sendDat[3] = 0x73;
	sendDat[4] = 0x2d;
	sendDat[5] = 0x6f;
	sendDat[6] = 0x6b;
	sendDat[7] = 0x0D;
	sendDat[8] = 0x0A; // �س�
	sendDat[9] = 0x3A; // ������־			
}
// ����updateTime�У�������⵱ǰʱ���Ƿ�ﵽԤ���ʱ��,�ﵽ���������
// Ϊ�˲�Ҫ1����һ�Σ����ǽ��������60���λ��ʱ����һ��
void checkAlarmTime()
{
	if((alarmMinute == minute) && (alarmHour == hour))
	{
		if((alarmMonth == month) && (alarmDay == day))
		{
			if(alarmYear == year)
			{
				buzzer_open();	
			}
		}	
	}
	else
	{
		buzzer_close();
		alarmFlag = 0; // �ر�����
	}
}
// �����õĽ����ݻ������ڵ��������ݷ��ͳ�ȥ
void sendAllSDAT()
{
	unsigned char i=0;
	for(i=0;i<64;i++)
	{
		SBUF = sdat[i];
		while(!TI);
		TI = 0;
	}
}

