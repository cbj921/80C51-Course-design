#ifndef __LED_H
#define __LED_H
// Ϊ��������������Ҫȡ����ʹ��
static unsigned char ledTable[] = {
	0x01,0x03,0x07,0x0f,
	0x1f,0x3f,0x7f,0xff
};

// ��ʾnum������led���������ڵ�λ��
void showLed (unsigned char num,unsigned char offset)
{
	unsigned char led = ledTable[num-1];
	led = led<<offset;
	P1 = ~led;
} 

#endif