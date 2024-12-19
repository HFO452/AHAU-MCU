#include <lcd1602.h>   		//����lcd1602�⣨�й���Ļ��ʾ�ı�
#include <reg51.h>			//����reg51��
#include <ds18b20.h>		//�����¶ȴ������Ŀ�

uint count=0; 				//����PWM����
uchar speed=0;				//��������PWM��ѭ������
sbit motor=P2^7; 			//���ڿ��Ʒ���
sbit light=P2^0;			//���ڿ��Ƽ��ȵƣ�D8��
sbit fmq=P1^4;			//���ڿ��Ʒ�����

void T_0(void) interrupt 1 using 1{
 	if(count<speed) motor=1;			//PWM������
 	if(count>speed) motor=0;
 	if(count>=5) count=0;	
	count++;
}

void main(void){			//������
	int rtemp;				//���ڴ洢�¶ȣ�
	char temp;				//���ڴ��¶ȴ�������ȡ�¶�
	bit addr=1;				//�����ж�Ҫ��at24c02���ȡ�����޸ĵ��ǵ����¶�����ֵ��0����������ֵ��1��
	bit change_can=0;		//�ж��Ƿ��������ģʽ
	char H,L;				//�洢���޺�����

	init_ds18b20();  		//��ʼ���¶ȴ�������ldc1602
	LCD_initial();	

	motor=0;

	TH0=0x66;
 	TL0=0x66;
 	TF0=0;
 	EA=1;
	ET0=1;
	/*
	string(0x80,"welcome to AAUCS");			//��ӭ����
	string(0xC0,"NO.22");	  
	delay(250);
	for(x=0;x<3;x++){
		write_command(0x08);
		delay(250);
		write_command(0x0c);
		delay(250);
	}
	for(x=0;x<14;x++){
		write_command(0x18);  
		delay(100);
	}
	write_command(0x01);
	delay(1);
	*/
	while(1){				//��ʼ����ɣ�����ѭ��			

		write_command(0xc0);//�ع��
							   
		change(0);	   		//lcd1602��һ����ʾ
		change(1);
	   

						 	//lcd1602�ڶ����¶���ʾ
		rtemp=retemp();
		temp=rtemp;
		if(rtemp==128)	temp=127;
		string(0xc0,"T:");
		if(rtemp>=1000)	write_data(0x31);
		else if(rtemp<0)	write_data(0x2d);
		else string(0xc2," ");
		if(rtemp>=100||rtemp<=-100)	write_data(0x30+(rtemp/100%10));
		else write_data(0x02);
		if(rtemp>=10||rtemp<=-10)	write_data(0x30+(rtemp/10%10));
		else	write_data(0x02);
		write_data(0x2e);
		write_data(0x30+rtemp%10);
	    write_data(0xdf); 
    	write_data(0x43); 

		H=read(0);			
		L=read(1);				

		if(addr)	string(0x90,"");						//lcd1602�ڶ��з���״̬
		else	string(0x83,"");
		temp=retemp()/10;
		if(temp>=H+5){
			speed=5;
			string(0xc9," FAN:3");
		}else if(temp>=H+3){
			speed=4;		   
			string(0xc9," FAN:2");
		}else if(temp>=H+1){	
			speed=2;	
			string(0xc9," FAN:1");
		}else{
			speed=0; 	   
			string(0xc9," FAN:0");
		}
		if(speed!=0)	TR0=1;
		else	TR0=0;

		if(change_can==1){
			if(addr)	string(0x89,"L:");
			else		string(0x82,"");
			write_command(0x0f);//�����  
			write_command(0x14);
			delay(100);	
			write_command(0x14);
			delay(100);	
			write_command(0x14);
			delay(100);	
		}

		write_ds(H+1,L+1);	  		//���¶����޺����ߴ����¶ȴ�����

		if(S1==0&&change_can==0){
			change_can=setup(); 	//�ж��Ƿ���S1�����ÿɽ�������ģʽ�������޸ģ�		
		}		

		if(change_can){    					//�����޸�
			if(S1==0){						//�ٴΰ���S1����������������
				addr=~addr;
			}  
			if(S2==0){		  			   	//�����õ�ֵ���м�һ������at24c02���¶ȴ�������������Ļ��ֵ
				write_ds(H+1,L+1);
				add(H,L,addr);
				delay(5);
			}else if(S3==0){			   	//�����õ�ֵ���м�һ������at24c02���¶ȴ�������������Ļ��ֵ
				write_ds(read(0),read(1));
				sub(H,L,addr);   
				delay(5);		   
				write_ds(H-1,L-1);
			}else if(S4==0){  			  	//�˳���ֵģʽ
			 	change_can=exit();
			}
		}

		if(temp<L+1)	light=0;
		else	light=1;
		
		if(H+1<=temp|L+1>temp)	fmq=0;
		else	fmq=1;	
			
	}
}