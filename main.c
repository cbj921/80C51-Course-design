#include <reg52.h>
#include "delay.h"
#include "buzzer.h"
#include "digitalTube.h"
#include "timer.h"
#include "key.h"
#include "led.h"

unsigned char keyFlag=0;
unsigned char countSecond = 0; // 用来计数3秒
unsigned char ledTime = 0;

// 通过中断来更新以下数据
unsigned char hour=0,minute=0,second=0,month=12,day=31,week = 0;
unsigned int year=2019; 
unsigned char T0Num = 0;
unsigned char modeFlag = 0; // 0：显示分秒 1：显示公元年  2：显示月日

void updateTimeData();
void showTime_MS();
void showTime_Year();
void stayThreeSecond();
void showTime_MonthDay();
void showWeekLed();
unsigned char getWeek();


int main()
{
	// init，各项功能初始化
	timerInit_0();           // 初始化计时器0
	week = getWeek();		 // 初始化星期
	showWeekLed();           // 显示LED
	///////////////////////
	while(1)
	{	
		updateTimeData();   // 更新时间数据
		modeFlag = checkKey_s1(&keyFlag,modeFlag); // 检测按键 S1
		modeFlag = checkKey_s2(&keyFlag,modeFlag); // 检测按键 S2
		switch(modeFlag){
			case 0: showTime_MS(); break;
			case 1: showTime_Year(); break;
			case 2: showTime_MonthDay(); break;
		}	
	}
	return 0;
}

// 定时器T0中断函数
void T0_func(void) interrupt 1
{
	// T0为50ms溢出一次
	// 重装计数器
	TH0 = 0x3c;    					//装定时初值高8位
	TL0 = 0xb0;    					//装定时初值低8位
	T0Num++;
	if(T0Num == 20)
	{
		T0Num = 0;
		second++;
		showWeekLed();             // 让LED灯显示并循环
	}
	stayThreeSecond();             // 三秒计数器	
}
// 更新时间数据
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
		week = getWeek();          //更新星期
	}
}
// 显示分秒的函数
void showTime_MS()
{
	unsigned char m1,m2,s1,s2;
	m2 = minute %10; // 分钟个位
	m1 = minute /10; // 分钟十位
	s2 = second %10; // 秒个位
	s1 = second /10; //	秒十位
	showFourNum(m1,m2,s1,s2,1);
}
// 显示公元年
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
// 显示月日
void showTime_MonthDay()
{
	unsigned char m1,m2,d1,d2;
	m2 = month %10;
	m1 = month /10;
	d2 = day %10;
	d1 = day /10;
	showFourNum(m1,m2,d1,d2,1);	
}
// 显示三秒的等待函数
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
// 计算星期函数
unsigned char getWeek()
{
	unsigned char topTwoYear = year/100;    // 年份前两位(默认年份四位数)
	unsigned char lastTwoYear = year%100;   // 年份后两位
	unsigned char dayGo = day;				// 日
	unsigned char monthGo = month;		    // 3<=monthGo<=14,即1，2月当作前年的13，14月
	unsigned char weekGo;

	if(monthGo<3)
	{
		if(monthGo == 1) monthGo = 13;
		else if(monthGo == 2) monthGo = 14;
	}

	weekGo = ((topTwoYear/4)-2*topTwoYear+lastTwoYear+(lastTwoYear/4)+((13*(monthGo+1))/5)+dayGo-1)%7;
	return weekGo;
}
// led显示函数和蜂鸣器鸣叫
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