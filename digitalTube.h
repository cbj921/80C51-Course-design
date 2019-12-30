#ifndef __DIGITAL_H
#define __DIGITAL_H
// 数码管段选编码数组
static unsigned char codeTable[] = {
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
	0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
};
// 数码管位选编码数组
static unsigned char maskTable[] = {
	0xfe,0xfd,0xfb,0xf7,0xef,0xdf
};
sbit dula = P2^6;
sbit wela = P2^7;

// 单位数字显示,第三个参数决定是否显示 点
void display(unsigned char mask,unsigned char num,unsigned char point)
{
	P0 = mask; 			 // 发送位选码
	wela = 1;            // 打开位选锁存器入口
	wela = 0;            // 锁存位选数据
	if(point)
	{
		P0 = 0x80|codeTable[num];
	}
	else
	{
		P0 = codeTable[num]; // 发送段选数据
	}
	dula = 1;			 // 打开段选锁存器入口
	dula = 0;			 // 锁存段选数据
}
// 显示六位数字
void showSixNum(unsigned char num1,unsigned char num2,unsigned char num3,unsigned char num4,unsigned char num5,unsigned char num6)
{
	// 显示完后要加延时函数，利用人视觉的暂留效应 
	display(maskTable[0],num1,0);
	delay_ms(1);
	display(maskTable[1],num2,1);
	delay_ms(1);
	display(maskTable[2],num3,0);
	delay_ms(1);
	display(maskTable[3],num4,1);
	delay_ms(1);
	display(maskTable[4],num5,0);
	delay_ms(1);
	display(maskTable[5],num6,0);
	delay_ms(1);
}
// 显示四位数字
void showFourNum(unsigned char num1,unsigned char num2,unsigned char num3,unsigned char num4,unsigned char point)
{
	// 显示完后要加延时函数，利用人视觉的暂留效应 
	display(maskTable[0],num1,0);
	delay_ms(1);
	if(point) display(maskTable[1],num2,1);
	else display(maskTable[1],num2,0);
	delay_ms(1);
	display(maskTable[2],num3,0);
	delay_ms(1);
	display(maskTable[3],num4,0);
	delay_ms(1);
}

#endif