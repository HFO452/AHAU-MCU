#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
#define out P0
sbit DQ=P1^5;
uchar temp;
/*
void delay5(uchar);				  	//延时5us
void init_ds18b20(void);			//初始化
uchar readbyte(void);				//读一个字节
void writebyte(uchar);				//写一个字节
int retemp(void);					//读温度
void write_ds(char H ,char L);		//写上下限
*/
void delay5(uchar n)
{
	do
	{
		_nop_();
		_nop_();
		_nop_();
		n--;
	}
	while(n);
}

void init_ds18b20(void)
{
	uchar x=0;
	DQ=0;
	delay5(120);
	DQ=1;
	delay5(16);
	delay5(80);
}

uchar readbyte(void)
{
	uchar i=0;
	uchar date=0;
	for(i=8;i>0;i--)
	{
		DQ=0;
		delay5(1);
		DQ=1;
		date>>=1;
		if(DQ)
		date|=0x80;
		delay5(11);
	}
	return(date);
}

void writebyte(uchar dat)
{
	uchar i=0;
	for(i=8;i>0;i--)
	{
		DQ=0;
		DQ=dat&0x01;
		delay5(12);
		DQ=1;
		dat>>=1;
		delay5(5);
	}
}

int retemp(void)
{
	uchar a,b;
	int t,tt;
	init_ds18b20();
	writebyte(0xcc);
	writebyte(0x44);
	init_ds18b20();
	writebyte(0xcc);
	writebyte(0xbe);
	a=readbyte();
	b=readbyte();  
	t=b;
	t<<=8;
	t=t|a;
	if(b>=8){
		t=~t+1;
		tt=t*(-0.625);
	}
	else	tt=t*0.625;
	return(tt);
}

void write_ds(char H ,char L){
	init_ds18b20();
	writebyte(0xcc);
	writebyte(0x4E);
	writebyte(H);
	writebyte(L);
}