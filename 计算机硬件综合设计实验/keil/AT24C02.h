#include <reg51.h>
#include <intrins.h>
#define	uchar unsigned char
#define uint unsigned int
sbit scl=P1^6;
sbit sda=P1^7;
/*
void start(void);				 	//��ʼ�ź�
void stop(void);					//��ֹ�ź�
void sack(void);					//���ͽ���Ӧ��λ
bit rack(void);						//����һ��Ӧ��λ
void ackn(void);					//���ͷǽ���Ӧ��λ
void send_byte(uchar);				//����1�ֽں���
uchar rec_byte(void);				//����1�ֽں���
void delay4us(void);				//��ʱ4us
uchar read(uchar addr);				//��addr�����¶�
void write(uchar x,uchar addr);		//��addrд���¶�
*/

void delay4us(void){
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

void start(void){
	scl=1;
	sda=1;
	delay4us();
	sda=0;
	delay4us();
	scl=0;
}

void stop(void){
	scl=0;
	sda=0;
	delay4us();
	scl=1;
	delay4us();
	sda=1;
	delay4us();
}

bit rack(void){
	bit flag;
	scl=1;
	delay4us();
	flag=sda;
	scl=0;
	return(flag);
}

void sack(void){
	sda=0;
	delay4us();
	scl=1;
	delay4us();
	scl=0;
	delay4us();
	sda=1;
	delay4us();
}

void ackn(void){
	sda=1;
	delay4us();
	scl=1;
	delay4us();
	scl=0;
	delay4us();
	sda=0;
}

uchar rec_byte(void){
	uchar i,temp;
	for(i=0;i<8;i++){
		temp<<=1;
		scl=1;
		delay4us();
		temp|=sda;
		scl=0;
		delay4us();
	}
	return(temp);
}

void send_byte(uchar temp){
	uchar i;
	scl=0;
	for(i=0;i<8;i++){
		sda=(bit)(temp&0x80);
		scl=1;
		delay4us();
		scl=0;
		temp<<=1;
	}
	sda=1;
}

uchar read(uchar addr){
	uchar i;
	bit f;
	start();
	send_byte(0xa0);
	f=rack();
	if(!f){
		send_byte(addr);
		f=rack();
		start();
		send_byte(0xa1);
		f=rack();
		i=rec_byte();
		ackn();
	}
	stop();
	return i;
}

void write(uchar x,uchar addr){
	bit f;
	start();
	send_byte(0xa0);
	f=rack();
	if(!f){
		send_byte(addr);
		f=rack();
		if(!f){
			send_byte(x);
			f=rack();
		}
	}
	stop();
	delay4us();
}
