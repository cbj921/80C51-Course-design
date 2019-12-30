#ifndef __BUZZER_H
#define __BUZZER_H
sbit BUZZER = P2^3;


void buzzer_open()
{
	BUZZER = 0;
}
void buzzer_close()
{
	BUZZER = 1;
}
void buzzer_reverse()
{
	BUZZER = ~BUZZER;
}

#endif