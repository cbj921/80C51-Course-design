#include <reg52.h>
#include "delay.h"
#include "buzzer.h"
#include "digitalTube.h"
#include "timer.h"
#include "key.h"
#include "led.h"
#include "uart.h"
// 注意事项，使用串口时一定要勾选 “发送新行” 否则出错；设置闹钟要将 “请于” 删除，直接以年份开头
//unsigned int test=0;
// 闹钟日期的变量
unsigned int alarmYear=0;
unsigned char alarmMonth=0,alarmDay=0,alarmHour=0,alarmMinute=0; 
unsigned char alarmFlag = 0; // 闹钟标志位 0：关闭  1：开启

unsigned char monthDay[] = {31,28,31,30,31,30,31,31,30,31,30,31}; // 二月暂时按平年算
unsigned char getData;
unsigned char sdat[64]={0x00};        // 数据缓存区
unsigned char sendDat[16]={0x00};     // 发送数据缓冲区
unsigned char sendIndex = 0;         // 发送缓冲区指针
unsigned char sp=0;   // 数据缓存区指针

unsigned char buzzerKeyFlag = 0;
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
		closeBuzzer_s3(&buzzerKeyFlag,&alarmFlag);			   // 检测按键 S3，若按下，则关闭蜂鸣器
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
		if(alarmFlag)
		{
			checkAlarmTime(); // 检测闹钟	
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
		week = getWeek(year,month,day);          //更新星期
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
	unsigned char length;
	// 接收数据
	if(RI)
	{
		// 一个数字或者英文字符 占用 一个字节
		// 一个汉字占用     两个字节
		RI = 0;
		getData = SBUF;
		sdat[sp] = getData;
		sp++;
		if(sp>=64) sp = 0;
		if(getData == 0x0A)
		{
			// 0x0A为结束标志位
			// 通过判断数组中有效长度来判断是校准还是答题模式
			length = getSDATLength(sdat);
			if(length == 19)
			{
				// 进行数据处理，赋值为对应的变量
				dealData(sdat);	
			}  
			else
			{
				// 进行字符分析，给出答案
				analyChar(sdat,length);
				sendData(sendDat);	
			} 
			// test：发送数据缓冲区全部内容
			//sendAllSDAT();
			// 清空sdat缓冲区，sp清零
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
	for(i=0;i<64;i++)
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
// 获得SDAT中有效数据长度
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
// 字符分析
void analyChar(unsigned char *point,unsigned char length)
{
	unsigned int tempYear=0;
	unsigned char tempMonth=0,tempDay=0,tempWeek=0;
	unsigned char tempHour=0,tempMinute=0;

	tempYear = getYearFromQuestion(point); // 获得年份
	if(length == 16)
	{
		// 回答是否为闰年
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
		sendDat[3] = 0x0A; // 回车
		sendDat[4] = 0x3A; // 0x3A为结束标志位
	}
	else if(length == 24)
	{
		// 回答是星期几
		tempMonth = getTimeFromData(&point[6]); // 获得月份
		tempDay = getTimeFromData(&point[10]); // 获得日
		tempWeek = getWeek(tempYear,tempMonth,tempDay);
		sendDat[0] = 0xD0;
		sendDat[1] = 0xC7; // 星
		sendDat[2] = 0xC6;
		sendDat[3] = 0xDA; // 期
		sendDat[4] = tempWeek + 0x30;
		sendDat[5] = 0x0D;
		sendDat[6] = 0x0A; // 回车
		sendDat[7] = 0x3A; // 0x3A为结束标志位		
	}
	else if(length == 22)
	{
		// 该问题少一个问号，为回答有几个工作日
		tempMonth = getTimeFromData(&point[6]); // 获得月份
		tempDay	  = getWorkDayFromMonth(tempYear,tempMonth); // 这里用tempDay作为工作日的变量
		sendDat[0] = (tempDay /10) +0x30;
		sendDat[1] = (tempDay %10) +0x30;
		sendDat[2] = 0xCC;
		sendDat[3] = 0xEC; // “天”
		sendDat[4] = 0x0D;
		sendDat[5] = 0x0A; // 回车
		sendDat[6] = 0x3A;  // 0x3A为结束标志位	
	}
	else if(length == 42) 
	{
		// 即拓展4，闹钟功能。
		// 要将 “请于” 删掉，直接从年份开始
		tempMonth = getTimeFromData(&point[6]); // 获得月份	 
		tempDay = getTimeFromData(&point[10]); // 获得日
		tempHour = getTimeFromData(&point[14]); // 获得小时
		tempMinute = getTimeFromData(&point[18]); // 获得分钟
		// 设置定时闹钟
		setAlarmClock(tempYear,tempMonth,tempDay,tempHour,tempMinute);
		//test
		/*	
		sendDat[0] = (tempMinute /10) +0x30;
		sendDat[1] = (tempMinute %10) +0x30;
		sendDat[2] = 0x3A;  // 0x3A为结束标志位
		*/	
	}	
}
// 从问题中获得年份
unsigned int getYearFromQuestion(unsigned char *point)
{
	// 因为年份就是前4位
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
// 发送发送缓冲区的内容
void sendData(unsigned char *point)
{
	unsigned char i;
	unsigned char *temp = point;
	// 因为发送缓冲区的数据是以 0x3A 来作为截止
	while(*point != 0x3A)
	{
		SBUF = *point;
		while(!TI);   // 当正在发送的时候等待 ,TI == 1 表示发送完成
		TI = 0;
		point++;
	}
	// 清除发送缓冲区
	for(i=0;i<16;i++)
	{
		temp[i] = 0x00;
	}		
}
// 从月份中获得工作日有几天
unsigned char getWorkDayFromMonth(unsigned int yearGo,unsigned char monthGo)
{
	// 注意年份都应该用 int 型，否则会溢出
	unsigned char tempWeek=0,tempDay=0;
	char workDay = 0;
	// 先获取该月的 1日是星期几
	tempWeek = getWeek(yearGo,monthGo,1);
	if(tempWeek == 0) tempWeek = 7;
	// 再获取该月一共有几天
	tempDay =  getMonthDay(yearGo,monthGo);
	// 计算工作日
	workDay += (6-tempWeek);
	if(workDay == -1) workDay = 0;
	// 计算剩余的天数
	tempDay -= (8 - tempWeek);
	// 计算剩余天数中的工作日
	workDay += (tempDay/7)*5; 
	tempDay = tempDay %7;
	if(tempDay >5) tempDay = 5;
	workDay += tempDay;

	return workDay;
}
// 设置定时闹钟
void setAlarmClock(unsigned int yearGo,unsigned char monthGo,unsigned char dayGo,unsigned char hourGo,unsigned char minuteGo)
{
	alarmYear = yearGo;
	alarmMonth = monthGo;
	alarmDay = dayGo;
	alarmHour = hourGo;
	alarmMinute = minuteGo;
	alarmFlag = 1; // 开启闹钟
	// 通过串口会应，告诉设置好了
	// 回答 it's-ok
	sendDat[0] = 0x49; 
	sendDat[1] = 0x74;
	sendDat[2] = 0x60;
	sendDat[3] = 0x73;
	sendDat[4] = 0x2d;
	sendDat[5] = 0x6f;
	sendDat[6] = 0x6b;
	sendDat[7] = 0x0D;
	sendDat[8] = 0x0A; // 回车
	sendDat[9] = 0x3A; // 结束标志			
}
// 用在updateTime中，用来检测当前时间是否达到预设的时间,达到则蜂鸣器响
// 为了不要1秒检测一次，我们将这个放在60秒进位的时候检测一次
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
		alarmFlag = 0; // 关闭闹钟
	}
}
// 测试用的将数据缓冲区内的所有内容发送出去
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

