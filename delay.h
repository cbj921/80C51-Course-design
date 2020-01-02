#ifndef __DELAY_H
#define __DELAY_H

void delay_ms(unsigned int ms)
{
	// ÑÓÊ±³ÌÐò
	unsigned int a,b; 
  	for(a=ms;a>0;a--)
  		for(b=848;b>0;b--);
}

void delay_us(unsigned int us)
{
	unsigned int a;
	for(a = us;a>0;a--);
}


#endif