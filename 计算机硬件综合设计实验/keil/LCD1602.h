#include <reg51.h>
#include <intrins.h>
#include <at24c02.h>
#define	uchar unsigned char
#define uint unsigned int
#define out P0
sbit RS=P1^0;
sbit RW=P1^1;
sbit E=P1^2;
sbit S1=P3^7;
sbit S2=P3^6;
sbit S3=P3^5;
sbit S4=P3^4;
/*
void lcd_initial(void);			//lcd初始化
void check_busy(void);			//检查忙碌
void write_command(uchar com);	//写指令
void write_data(uchar dat);		//写数据
void string(uchar ad,uchar *s);	//指定位置写字符串
void delay(uint);				//延迟
bit setup(void);				//设置
void add(char H,char L,uchar addr);	//加一
void sub(char H,char L,uchar addr);	//减一
bit exit(void);					//退出设置
void change(addr);				//重写lcd1602的上限或下限值
*/
void delay(uint j){
	uchar i=250;
	for(;j>0;j--){
		while(--i);
		i=249;
		while(--i);
		i=250;
	}
}

void check_busy(void){
	uchar dt;
	do{
		dt=0xff;
		E=0;
		RS=0;
		RW=1;
		E=1;
		dt=out;
	}while(dt&0x80);
	E=0;
}

void write_command(uchar com){
	check_busy();
	RS=0;
	RW=0;
	E=0;
	out=com;
	E=1;
	_nop_();
	E=0;
	delay(1);
}

void write_data(uchar dat){
	check_busy();
	RS=1;
	RW=0;
	E=0;
	out=dat;
	E=1;
	_nop_();
	E=0;
	delay(5);
}

void LCD_initial(void){
	write_command(0x38);
	_nop_();
	write_command(0x0C);
	_nop_();
	write_command(0x06);
	_nop_();
	write_command(0x01);

	delay(1);
}

void string(uchar ad,uchar *s){
	write_command(ad);
	while(*s>0){
		write_data(*s++);
		delay(1);
	}
}

bit setup(void){
	return 1;
}

void add(char H,char L,uchar addr){
	bit b=0;
	if((H>=L&&addr==0)|(H>L&&addr==1))	b=1;
	check_busy();
	if(addr?L:H<127&&b){
		if(addr)	write(L+1,addr);
		else	write(H+1,addr);
	}
}

void sub(char H,char L,uchar addr){ 
	bit b=0;
	if((H>L&&addr==0)|(H>=L&&addr==1))	b=1; 
	check_busy();
	if(addr?L:H>-55&&b){
	 	if(addr)	write(L-1,addr);
		else	write(H-1,addr);
	}
}

bit exit(void){		
	return 0;
}

void change(addr){
	char x;
	int temp;
	x=read(addr);
	temp=x+1;	
	if(addr==0)	string(0x80,"H:");
	else string(0x89,"L:");
	if(temp>=100){
		write_data(0x31);
	}else if(temp<0){
		write_data(0x2d);
		temp=-temp;
	}else{
		write_data(0x20);
	}
	if(temp<10)	write_data(0x20);
	else if(temp>=100)	write_data(0x30+temp/10%10);
	else write_data(0x30+temp/10);
	write_data(0x30+temp%10);
	write_data(0xdf);
	write_data(0x43);
	if(addr==0)	string(0x82,"");
	else	string(0x90,"");
}