#ifndef __UART_H
#define __UART_H
#define XTAL 11059200
#define BAUDRATE 9600

uart_init()
{
	TMOD |= 0X20;
	TH1 = (unsigned char)(256-(XTAL/(32L*12L*BAUDRATE))); // SMOD = 0
	TL1 = (unsigned char)(256-(XTAL/(32L*12L*BAUDRATE))); // 32和12后面一定要加后缀L，或者通信不了
	SCON |= 0x50;    // REN位是用来允许串口接收数据的
	PCON |= 0X00;
	ES = 1;
	ET1 = 0;
   	TR1 = 1;	
}

#endif