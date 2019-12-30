#ifndef __DIGITAL_H
#define __DIGITAL_H
// ����ܶ�ѡ��������
static unsigned char codeTable[] = {
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
	0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
};
// �����λѡ��������
static unsigned char maskTable[] = {
	0xfe,0xfd,0xfb,0xf7,0xef,0xdf
};
sbit dula = P2^6;
sbit wela = P2^7;

// ��λ������ʾ,���������������Ƿ���ʾ ��
void display(unsigned char mask,unsigned char num,unsigned char point)
{
	P0 = mask; 			 // ����λѡ��
	wela = 1;            // ��λѡ���������
	wela = 0;            // ����λѡ����
	if(point)
	{
		P0 = 0x80|codeTable[num];
	}
	else
	{
		P0 = codeTable[num]; // ���Ͷ�ѡ����
	}
	dula = 1;			 // �򿪶�ѡ���������
	dula = 0;			 // �����ѡ����
}
// ��ʾ��λ����
void showSixNum(unsigned char num1,unsigned char num2,unsigned char num3,unsigned char num4,unsigned char num5,unsigned char num6)
{
	// ��ʾ���Ҫ����ʱ�������������Ӿ�������ЧӦ 
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
// ��ʾ��λ����
void showFourNum(unsigned char num1,unsigned char num2,unsigned char num3,unsigned char num4,unsigned char point)
{
	// ��ʾ���Ҫ����ʱ�������������Ӿ�������ЧӦ 
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