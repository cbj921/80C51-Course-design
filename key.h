#ifndef __KEY_H
#define __KEY_H
#include "delay.h"
#include "buzzer.h"

sbit s1 = P3^4;
sbit s2 = P3^5;
sbit s3 = P3^6;

unsigned char checkKey_s1(unsigned char *keyFlag,unsigned char modeFlag)
{
	if(s1 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s1 == 0)
		{
			*keyFlag = 1;	  // 在显示的三秒中不响应按键
			return 1;
		}
	}
	return modeFlag;
}

unsigned char checkKey_s2(unsigned char *keyFlag,unsigned char modeFlag)
{
	if(s2 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s2 == 0)
		{
			*keyFlag = 1;	  // 在显示的三秒中不响应按键
			return 2;
		}
	}
	return modeFlag;
}

void closeBuzzer_s3(unsigned char *keyFlag)
{
	if(s3 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s3 == 0)
		{
			*keyFlag = 1;
			buzzer_close();
		}
	}
	else if(s3 == 1 && *keyFlag == 1)
	{
		*keyFlag = 0;
	}
}

#endif