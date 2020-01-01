#ifndef __LED_H
#define __LED_H
// 为灯亮的数量，需要取反后使用
static unsigned char ledTable[] = {
	0x01,0x03,0x07,0x0f,
	0x1f,0x3f,0x7f,0xff
};
unsigned char data_rl(unsigned char x,unsigned char y,unsigned char z);


// 显示num数量的led，和他所在的位置
void showLed (unsigned char num,unsigned char offset)
{
	unsigned char led = ledTable[num-1];
	led = data_rl(led,8,offset);
	P1 = ~led;
} 
// 循环左移函数
unsigned char data_rl(unsigned char x,unsigned char y,unsigned char z)
{
	unsigned char temp;
    temp=x>>(y-z);
    x=x<<z;
    x=x | temp;
    return x;
}
// 熄灭全部LED
void clearAllLed()
{
	P1 = 0xFF;
}

#endif