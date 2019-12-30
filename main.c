#include <reg52.h>
#include "delay.h"
#include "buzzer.h"
#include "digitalTube.h"
#include "timer.h"
#include "key.h"
#include "led.h"

unsigned char keyFlag=0;
unsigned char countSecond = 0; // ��������3��
unsigned char ledTime = 0;

// ͨ���ж���������������
unsigned char hour=0,minute=0,second=0,month=12,day=31,week = 0;
unsigned int year=2019; 
unsigned char T0Num = 0;
unsigned char modeFlag = 0; // 0����ʾ���� 1����ʾ��Ԫ��  2����ʾ����

void updateTimeData();
void showTime_MS();
void showTime_Year();
void stayThreeSecond();
void showTime_MonthDay();
void showWeekLed();
unsigned char getWeek();


int main()
{
	// init������ܳ�ʼ��
	timerInit_0();           // ��ʼ����ʱ��0
	week = getWeek();		 // ��ʼ������
	showWeekLed();           // ��ʾLED
	///////////////////////
	while(1)
	{	
		updateTimeData();   // ����ʱ������
		modeFlag = checkKey_s1(&keyFlag,modeFlag); // ��ⰴ�� S1
		modeFlag = checkKey_s2(&keyFlag,modeFlag); // ��ⰴ�� S2
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
		week = getWeek();          //��������
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
	unsigned tempYear = year;
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
unsigned char getWeek()
{
	unsigned char topTwoYear = year/100;    // ���ǰ��λ(Ĭ�������λ��)
	unsigned char lastTwoYear = year%100;   // ��ݺ���λ
	unsigned char dayGo = day;				// ��
	unsigned char monthGo = month;		    // 3<=monthGo<=14,��1��2�µ���ǰ���13��14��
	unsigned char weekGo;

	if(monthGo<3)
	{
		if(monthGo == 1) monthGo = 13;
		else if(monthGo == 2) monthGo = 14;
	}

	weekGo = ((topTwoYear/4)-2*topTwoYear+lastTwoYear+(lastTwoYear/4)+((13*(monthGo+1))/5)+dayGo-1)%7;
	return weekGo;
}
// led��ʾ�����ͷ���������
void showWeekLed()
{
	if(week !=0)
	{
		showLed(week,ledTime%8);
		ledTime++;	
	}
	else
	{
		buzzer_reverse();
	}
}