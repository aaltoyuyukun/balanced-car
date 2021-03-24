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

sbit IR=P3^2;  					//����ӿڱ�־
bit irpro_ok,irok;
unsigned char IRcord[4];
unsigned char irdata[33];
unsigned char Ir_work(void);	//�����ֵɢת����
void Ircordpro(void);

																
unsigned char an,timerf=0; 	////timerf���ڶ�ʱ��2�ж�ˢ����Ļ����
unsigned char vdate=1;				        	////vdateλ�������ת��
unsigned char  irtime;//������ȫ�ֱ���
bit flog=1,dj=0,lr=1;            		
sbit LED = P2^0;																	

void main ()
{ 

 Ds1302_Init();
 LCD_Init() ; ///1602��ʼ��
 Init_Timer2();	///����ɨ�谴��,�Դ���������
 Init_Timer1();	///����ˢ����Ļʱ��
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
		LCD_Write_String(1,0,"Dianji V="); //////�������ת����ʾ
		LCD_Write_String(1,1,"T   :  :"); 	   //////ʱ����ʾ
 		Show(3,1,time_buf1[4]);
 		Show(6,1,time_buf1[5]);
  		Show(9,1,time_buf1[6]);
		if(dj)
		 LCD_Write_String(14,0,"ON");
		else
		 LCD_Write_String(14,0,"OF"); //////�������ת����ʾ//m
		if(lr)
		 LCD_Write_String(12,1,"RIGHT");
		else
		 LCD_Write_String(12,1,"LIFT"); //////�������ת����ʾ//m
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
/*                  �ⲿ�ж�0��ʼ��
------------------------------------------------*/
void EX0init(void)
{
 IT0 = 1;   //ָ���ⲿ�ж�0�½��ش�����INT0 (P3.2)
 EX0 = 1;   //ʹ���ⲿ�ж�
 EA = 1;    //�����ж�
}
////////////////��ʱ��012��ʼ�����򡢡�����������
void Init_Timer0(void)
{
  TMOD=0x02;//��ʱ��0������ʽ2��TH0����װֵ��TL0�ǳ�ֵ
  TH0=0x00; //����ֵ
  TL0=0x00; //��ʼ��ֵ
  ET0=1;    //���ж�
  TR0=1;
}

void Init_Timer1(void)
{
 TMOD |= 0x10;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
 TH1=0x40;	      //������ֵ
 TL1=0x00;
// EA=1;            //���жϴ�
 ET1=1;           //��ʱ���жϴ�
 TR1=1;           //��ʱ�����ش�
}
void Init_Timer2(void)
{
 RCAP2H=0x50;	      //������ֵ
 RCAP2L=0x00;
 ET2=1;
// EA=1;            //���жϴ�
 TR2=1;           //����ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
}
			//////////�жϳ���////////
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
  irtime++;  //���ڼ���2���½���֮���ʱ��
}

void EX0_ISR (void) interrupt 0 //�ⲿ�ж�0������
{
  static unsigned char  i;             //���պ����źŴ���
  static bit startflag;                //�Ƿ�ʼ�����־λ

if(startflag)                         
   {
    if(irtime<63&&irtime>=33)//������ TC9012��ͷ�룬9ms+4.5ms
                        i=0;
    		irdata[i]=irtime;//�洢ÿ����ƽ�ĳ���ʱ�䣬�����Ժ��ж���0����1
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
     if(irok)                        //������պ��˽��к��⴦��
	  	 {   
	   	 Ircordpro();
 	   	 irok=0;irpro_ok=1;//������ϱ�־λ��1

	  	 }
	 if(irpro_ok)                   //�������ú���й��������簴��Ӧ�İ�������ʾ��Ӧ�����ֵ�
	 	 {
		  irpro_ok=0;//������ɱ�־
	   	  an=Ir_work();LED=!LED;
  		 }

}



void Ircordpro(void)//������ֵ������
{ 
  unsigned char i, j, k;
  unsigned char cord,value;

  k=1;
  for(i=0;i<4;i++)      //����4���ֽ�
     {
      for(j=1;j<=8;j++) //����1���ֽ�8λ
         {
          cord=irdata[k];
          if(cord>7)//����ĳֵΪ1������;����о��Թ�ϵ������ʹ��12M���㣬��ֵ������һ�����
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

unsigned char Ir_work(void)//�����ֵɢת����
{
       switch(IRcord[2])//�жϵ���������ֵ
	         {
			 case 0x0c:return 1;break;//1 ��ʾ��Ӧ�İ���ֵ
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
				