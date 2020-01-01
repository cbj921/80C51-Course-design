#ifndef __UART_H
#define __UART_H
#define XTAL 11059200
#define BAUDRATE 9600

uart_init()
{
	TMOD |= 0X20;
	TH1 = (unsigned char)(256-(XTAL/(32L*12L*BAUDRATE))); // SMOD = 0
	TL1 = (unsigned char)(256-(XTAL/(32L*12L*BAUDRATE))); // 32��12����һ��Ҫ�Ӻ�׺L������ͨ�Ų���
	SCON |= 0x50;    // RENλ�����������ڽ������ݵ�
	PCON |= 0X00;
	ES = 1;
	ET1 = 0;
   	TR1 = 1;	
}

#endif