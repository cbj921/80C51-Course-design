#ifndef __KEY_H
#define __KEY_H
#include "delay.h"
#include "buzzer.h"

sbit s1 = P3^4;
sbit s2 = P3^5;
sbit s3 = P3^6;
sbit s4 = P3^7;

unsigned char checkKey_s1(unsigned char *keyFlag,unsigned char modeFlag)
{
	if(s1 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s1 == 0)
		{
			*keyFlag = 1;	  // ����ʾ�������в���Ӧ����
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
			*keyFlag = 1;	  // ����ʾ�������в���Ӧ����
			return 2;
		}
	}
	return modeFlag;
}
// ���ӵķ������ر�
void closeBuzzer_s3(unsigned char *keyFlag,unsigned char *alarmFlag)
{
	if(s3 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s3 == 0)
		{
			*keyFlag = 1;
			*alarmFlag = 0;
			buzzer_close();
		}
	}
	else if(s3 == 1 && *keyFlag == 1)
	{
		*keyFlag = 0;
	}
}
// ����ܰ���
unsigned char openStopWatch_s4(unsigned char *stopWatchKeyFlag,unsigned char modeFlag)
{
	if(s4 == 0 && *stopWatchKeyFlag == 0)
	{
		delay_ms(1);
		if(s4 == 0)
		{
			*stopWatchKeyFlag = 1;
			return 3;
		}
	}
	return modeFlag;	
}
// ���ģʽ�У���⿪ʼ�İ���
unsigned char check_stopWatch_start(unsigned char *keyFlag,unsigned char startFlag)
{
	if(s1 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s1 == 0)
		{
			*keyFlag = 1;
			return 1;
		}
	}
	else if(s1 == 1 && *keyFlag == 1)
	{
		*keyFlag = 0;
	}
	return startFlag;
}
// ���ģʽ�У��������İ���
unsigned char clear_stopWatch_time(unsigned char *keyFlag,unsigned char startFlag,unsigned char *minute,unsigned char *second,unsigned int *ms)
{
	if(s2 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s2 == 0)
		{
			*keyFlag = 1;
			*minute = 0;
			*second = 0;
			*ms = 0;
			return 0;
		}
	}
	else if(s2 == 1 && *keyFlag == 1)
	{
		*keyFlag = 0;
	}
	return startFlag;	
}
// ��ⰴ��S4�����ʹ�������
unsigned char stopWatch_check_s4(unsigned char *keyFlag)
{
	if(s4 == 0 && *keyFlag == 0)
	{
		delay_ms(1);
		if(s4 == 0)
		{
			*keyFlag = 1;
			return 1;
		}
	}
	else if(s4 == 1 && *keyFlag == 1)
	{
		*keyFlag = 0;
	}
	return 0;	
}
// ���ģʽ�У�����˳��İ���
unsigned char check_stopWatch_quit(unsigned char startFlag,unsigned char *modeFlag,unsigned char *stopWatchKeyFlag,unsigned char *quitFlag)
{
	if(s3 == 0 && *quitFlag == 0)
	{
		delay_ms(1);
		if(s3 == 0)
		{
			*quitFlag = 1;
			*modeFlag = 0;
			*stopWatchKeyFlag = 0;
			return 0;
		}
	}
	else if(s3 == 1 && *quitFlag == 1)
	{
		*quitFlag = 0;
	}
	return startFlag;
}

#endif