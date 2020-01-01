#include <reg52.h>
#include "delay.h"
#include "buzzer.h"
#include "digitalTube.h"
#include "timer.h"
#include "key.h"
#include "led.h"
#include "uart.h"

unsigned int test=0;

unsigned char monthDay[] = {31,28,31,30,31,30,31,31,30,31,30,31}; // 二月暂时按平年算
unsigned char getData;
unsigned char sdat[32]={0x00}; // 数据缓存区
unsigned char sp=0;   // 数据缓存区指针

unsigned char keyFlag=0;
unsigned char countSecond = 0; // 用来计数3秒
unsigned char ledTime = 0;

// 通过中断来更新以下数据
unsigned char hour=0,minute=0,second=0,month=1,day=21,week = 0;
unsigned int year=2020; 
unsigned char T0Num = 0;
unsigned char modeFlag = 0; // 0：显示分秒 1：显示公元年  2：显示月日

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
unsigned char getMonthDay(unsigned char tMonth);

int main()
{
	// init，各项功能初始化
	timerInit_0();           // 初始化计时器0
	uart_init();             // 初始化串口
	week = getWeek(year,month,day);		 // 初始化星期
	showWeekLed();           // 显示LED
	clearSDAT();             // 清空缓冲区
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
		week = getWeek(year,month,day);          //更新星期
	}
	if(day > getMonthDay(month))
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
unsigned char getWeek(unsigned int tYear,unsigned char tMonth,unsigned char tDay)
{
	unsigned char topTwoYear ;              // 年份前两位(默认年份四位数)
	unsigned char lastTwoYear ;             // 年份后两位
	unsigned char dayGo = tDay;				// 日
	unsigned char monthGo = tMonth;		    // 3<=monthGo<=14,即1，2月当作前年的13，14月
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
// led显示函数和蜂鸣器鸣叫
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
		buzzer_reverse();	// 太吵了暂时注释
	}
}

// 串口中断函数
void uart_func(void) interrupt 4
{
	// 接收数据
	if(RI)
	{
		// 一个数字或者英文字符 占用 一个字节
		// 一个汉字占用     两个字节
		RI = 0;
		getData = SBUF;
		sdat[sp] = getData;
		sp++;
		if(sp>=32) sp = 0;
		if(getData == 0x0A)
		{
			// 0x0A为结束标志位
			// TODO: 进行数据处理，赋值为对应的变量
			ES = 0;
			dealData(sdat);
			// TODO: 清空sdat缓冲区，sp清零
			clearSDAT();
			sp = 0;
			ES = 1;
		}
		//SBUF = getData;
		//test
	}
	if(TI)
	{
		TI =0;
	}
}
// 处理接收来的数据
void dealData(unsigned char *dataGo)
{
	unsigned char timeFlag = 0;  // 处理时间的标志位，0：年，1：月，2：日，以此类推
	unsigned char temp=0,i;
	// 将原时间数据都清零
	year = 0,month = 0,day = 0,hour = 0,minute = 0,second = 0;

	while(*dataGo != 0x0D)
	{
		// 处理日期数据
		if(*dataGo == 0x2F || *dataGo == 0x3A ||*dataGo == 0x2D) // 因为斜杆和冒号的十六进制为 0x2f和0x3a
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
				month = getTimeFromData(dataGo); // 因为参数只是传值，不会改变真正的datago地址
				dataGo += 2;					 // 所以我们要在函数外面再自增 2 才行
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
// 计算月日时分秒的函数
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
// 清空缓冲区
void clearSDAT()
{
	unsigned char i;
	for(i=0;i<32;i++)
	{
		sdat[i] = 0x00;
	}
}
// 判断平润年
unsigned char judgeLeapOrOrdinaryYear(unsigned int tYear)
{
	unsigned int tempYear = tYear;
	if((tempYear%4 == 0 && tempYear%100 !=0)||(tempYear%100 == 0 && tempYear%400==0))
	{
		return 1;	
	}
	return 0;
}
// 得到相应月份有几天
unsigned char getMonthDay(unsigned char tMonth)
{
	if(tMonth == 2)
	{
		if(judgeLeapOrOrdinaryYear(tMonth))
		{
			return 29;
		}
	}
	return monthDay[tMonth-1];		
}