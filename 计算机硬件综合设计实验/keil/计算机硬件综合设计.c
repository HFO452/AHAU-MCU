#include <lcd1602.h>   		//导入lcd1602库（有关屏幕显示改变
#include <reg51.h>			//导入reg51库
#include <ds18b20.h>		//导入温度传感器的库

uint count=0; 				//用作PWM计数
uchar speed=0;				//用作控制PWM的循环次数
sbit motor=P2^7; 			//用于控制风扇
sbit light=P2^0;			//用于控制加热灯（D8）
sbit fmq=P1^4;			//用于控制蜂鸣器

void T_0(void) interrupt 1 using 1{
 	if(count<speed) motor=1;			//PWM波设置
 	if(count>speed) motor=0;
 	if(count>=5) count=0;	
	count++;
}

void main(void){			//主函数
	int rtemp;				//用于存储温度；
	char temp;				//用于从温度传感器读取温度
	bit addr=1;				//用于判断要从at24c02里读取或者修改的是的是温度上限值（0）还是下限值（1）
	bit change_can=0;		//判断是否进入设置模式
	char H,L;				//存储上限和下限

	init_ds18b20();  		//初始化温度传感器和ldc1602
	LCD_initial();	

	motor=0;

	TH0=0x66;
 	TL0=0x66;
 	TF0=0;
 	EA=1;
	ET0=1;
	/*
	string(0x80,"welcome to AAUCS");			//欢迎界面
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
	while(1){				//初始化完成，进入循环			

		write_command(0xc0);//关光标
							   
		change(0);	   		//lcd1602第一行显示
		change(1);
	   

						 	//lcd1602第二行温度显示
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

		if(addr)	string(0x90,"");						//lcd1602第二行风扇状态
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
			write_command(0x0f);//开光标  
			write_command(0x14);
			delay(100);	
			write_command(0x14);
			delay(100);	
			write_command(0x14);
			delay(100);	
		}

		write_ds(H+1,L+1);	  		//将温度上限和下线存入温度传感器

		if(S1==0&&change_can==0){
			change_can=setup(); 	//判断是否按下S1，设置可进入设置模式（进行修改）		
		}		

		if(change_can){    					//进行修改
			if(S1==0){						//再次按下S1，更换设置上下限
				addr=~addr;
			}  
			if(S2==0){		  			   	//对设置的值进行加一，存入at24c02和温度传感器，更改屏幕数值
				write_ds(H+1,L+1);
				add(H,L,addr);
				delay(5);
			}else if(S3==0){			   	//对设置的值进行减一，存入at24c02和温度传感器，更改屏幕数值
				write_ds(read(0),read(1));
				sub(H,L,addr);   
				delay(5);		   
				write_ds(H-1,L-1);
			}else if(S4==0){  			  	//退出设值模式
			 	change_can=exit();
			}
		}

		if(temp<L+1)	light=0;
		else	light=1;
		
		if(H+1<=temp|L+1>temp)	fmq=0;
		else	fmq=1;	
			
	}
}