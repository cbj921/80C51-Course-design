#ifndef __TIMER_H
#define __TIMER_H

// 定时器0用来计时 1秒 ，初始化为 50ms 触发中断函数一次

void timerInit_0()
{
	TMOD = 0x01;                	//设置定时器 0 工作工作模式为 1
	TH0 = 0x3c;    					//装定时初值高8位
	TL0 = 0xb0;    					//装定时初值低8位
	EA = 1;                     	//开总中断
	ET0 = 1;	                   	//开定时器0中断						
	TR0 = 1;	                   	//启动定时器0	
}


#endif
