 #include <reg52.h>
 #include "1602.h"
 #include "timer.h"
// #include "bujindianji.h"
// #include "4anjian.h"

void bujin(bit on,bit lifrit);
void Init_Timer2(void);
void Init_Timer1(void);
void Init_Timer0(void);
void EX0init(void);

sbit IR=P3^2;  					//红外接口标志
bit irpro_ok,irok;
unsigned char IRcord[4];
unsigned char irdata[33];
unsigned char Ir_work(void);	//红外键值散转程序
void Ircordpro(void);

																
unsigned char an,timerf=0; 	////timerf用于定时器2中断刷新屏幕计数
unsigned char vdate=1;				        	////vdate位步进电机转速
unsigned char  irtime;//红外用全局变量
bit flog=1,dj=0,lr=1;            		
sbit LED = P2^0;																	

void main ()
{ 

 Ds1302_Init();
 LCD_Init() ; ///1602初始化
 Init_Timer2();	///用于扫描按键,自带按键喇叭
 Init_Timer1();	///用于刷新屏幕时间
 Init_Timer0();
 EX0init();

// Ds1302_Write_Time();
 while (1)
 {
 	if(flog)
	 {
	 Ds1302_Read_Time();
		LCD_Clear();
		Show(10,0,vdate);		
		LCD_Write_String(1,0,"Dianji V="); //////步进电机转速显示
		LCD_Write_String(1,1,"T   :  :"); 	   //////时间显示
 		Show(3,1,time_buf1[4]);
 		Show(6,1,time_buf1[5]);
  		Show(9,1,time_buf1[6]);
		if(dj)
		 LCD_Write_String(14,0,"ON");
		else
		 LCD_Write_String(14,0,"OF"); //////步进电机转速显示//m
		if(lr)
		 LCD_Write_String(12,1,"RIGHT");
		else
		 LCD_Write_String(12,1,"LIFT"); //////步进电机转速显示//m
		flog=0;
	  }
		if(an==1)
		 {
		 vdate=vdate+1;
		 an=0;
		  if (vdate>10)
		    vdate=10;
		 }
//		if(an==11)
//		 {vdate=vdate+5;an=0;}
		if(an==2)
		 {
		 vdate=vdate-1;
		 an=0;
		  if(vdate==0)
		   vdate=1;
		 }
//		if(an==22)
//		 {vdate=vdate-5;an=0;}
		if(an==3)
		 {dj=!dj;an=0;}
		if(an==4)
		 {lr=!lr;an=0;}
				  
  }
}
/*                  外部中断0初始化
------------------------------------------------*/
void EX0init(void)
{
 IT0 = 1;   //指定外部中断0下降沿触发，INT0 (P3.2)
 EX0 = 1;   //使能外部中断
 EA = 1;    //开总中断
}
////////////////定时器012初始化程序、、、、、、、
void Init_Timer0(void)
{
  TMOD=0x02;//定时器0工作方式2，TH0是重装值，TL0是初值
  TH0=0x00; //重载值
  TL0=0x00; //初始化值
  ET0=1;    //开中断
  TR0=1;
}

void Init_Timer1(void)
{
 TMOD |= 0x10;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 TH1=0x40;	      //给定初值
 TL1=0x00;
// EA=1;            //总中断打开
 ET1=1;           //定时器中断打开
 TR1=1;           //定时器开关打开
}
void Init_Timer2(void)
{
 RCAP2H=0x50;	      //给定初值
 RCAP2L=0x00;
 ET2=1;
// EA=1;            //总中断打开
 TR2=1;           //定用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
}
			//////////中断程序////////
void Timer2(void) interrupt 5 
{
  
  TF2=0;
  an=KeyScan();
  if(timerf>10)
   {
   flog=1;
   timerf=0;
   }
  else
   timerf++;
}

void Timer1(void) interrupt 3 
{
 if(vdate==10)
  TH1=231;
 else if(vdate==9)
   TH1=230;
    else if(vdate==8)
   TH1=229;
 else if(vdate==7)
   TH1=227;
 else if(vdate==6)
   TH1=225;
 else if(vdate==5)
   TH1=220;
 else if(vdate==4)
   TH1=210;
  else if(vdate==3)
   TH1=200;
 else if(vdate==2)
   TH1=150;
 else if (vdate==1)
   TH1=50;

 TL1=0x00;
 bujin(dj,lr);  	 
}



void timer0 (void) interrupt 1
{
  irtime++;  //用于计数2个下降沿之间的时间
}

void EX0_ISR (void) interrupt 0 //外部中断0服务函数
{
  static unsigned char  i;             //接收红外信号处理
  static bit startflag;                //是否开始处理标志位

if(startflag)                         
   {
    if(irtime<63&&irtime>=33)//引导码 TC9012的头码，9ms+4.5ms
                        i=0;
    		irdata[i]=irtime;//存储每个电平的持续时间，用于以后判断是0还是1
    		irtime=0;
    		i++;
   			 if(i==33)
      			{
	  			 irok=1;
				 i=0;
	  			}
          }
  	 else
		{
		irtime=0;
		startflag=1;
		}
     if(irok)                        //如果接收好了进行红外处理
	  	 {   
	   	 Ircordpro();
 	   	 irok=0;irpro_ok=1;//处理完毕标志位置1

	  	 }
	 if(irpro_ok)                   //如果处理好后进行工作处理，如按对应的按键后显示对应的数字等
	 	 {
		  irpro_ok=0;//处理完成标志
	   	  an=Ir_work();LED=!LED;
  		 }

}



void Ircordpro(void)//红外码值处理函数
{ 
  unsigned char i, j, k;
  unsigned char cord,value;

  k=1;
  for(i=0;i<4;i++)      //处理4个字节
     {
      for(j=1;j<=8;j++) //处理1个字节8位
         {
          cord=irdata[k];
          if(cord>7)//大于某值为1，这个和晶振有绝对关系，这里使用12M计算，此值可以有一定误差
             value|=0x80;
          if(j<8)
		    {
			 value>>=1;
			}
           k++;
         }
     IRcord[i]=value;
     value=0;     
     } 
}

unsigned char Ir_work(void)//红外键值散转程序
{
       switch(IRcord[2])//判断第三个数码值
	         {
			 case 0x0c:return 1;break;//1 显示相应的按键值
			 case 0x18:return 2;break;//2
			 case 0x5e:return 3;break;//3
			 case 0x08:return 4;break;//4
			 case 0x1c:return 5;break;//5
			 case 0x5a:return 6;break;//6
			 case 0x42:return 7;break;//7
			 case 0x52:return 8;break;//8
			 case 0x4a:return 9;break;//9
             default:break;
			 }

		  

  }
				