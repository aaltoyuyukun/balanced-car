//****************************************
// Update to MPU6050 by shinetop
// MCU: STC89C52
// 2012.3.1
// ����: ��ʾ���ٶȼƺ������ǵ�10λԭʼ����
//****************************************
// GY-52 MPU3050 IIC���Գ���
// ʹ�õ�Ƭ��STC89C51 
// ����11.0592M
// ��ʾ��LCD1602
// ���뻷�� Keil uVision2
// �ο��꾧��վ24c04ͨ�ų���
// ʱ�䣺2011��9��1��
// QQ��531389319
//****************************************
#include <REG52.H>	
#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library	
#include <INTRINS.H>
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
//****************************************
// ����51��Ƭ���˿�
//****************************************
#define DataPort P0		//LCD1602���ݶ˿�
sbit    SCL=P1^0;			//IICʱ�����Ŷ���
sbit    SDA=P1^1;			//IIC�������Ŷ���
sbit    LCM_RS=P2^4;		//LCD1602����˿�		
sbit    LCM_RW=P2^5;		//LCD1602����˿�		
sbit    LCM_EN=P2^6;		//LCD1602����˿� 
sbit maichong=P1^2; //������ʲ���������Ӷ˿�
sbit dir=P1^3;
//****************************************
// ����MPU6050�ڲ���ַ
//****************************************
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I			0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define	SlaveAddress	0xD0	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ
//****************************************
//�������ͼ�����
//****************************************
uchar dis[4];							//��ʾ����(-511��512)���ַ�����
unsigned int speed;
int	dis_data;	
static unsigned int  Kp=10,KI,KD,Ti=10,Td=5;//���ж���Kp,Ti,Td;; //��������Proportional Const
double  Ki,Kd,T=0.001; 
double error=0,error_1=0,error_2=0; //Error[-1]
double E0=23,E1=0,E2=0; //�趨Ŀ��Desired Value ����T����Ϊ1ms					//����
double P_now=0,P_last1=0,P_last2=0;
double Xjiaodu;
unsigned char num;
unsigned int time2=0,times1=0,times2=0;
static unsigned int tun0=0,tun1=0,tun3=0,tun4=0;
//****************************************
//��������
//****************************************
void  delay(unsigned int k);
void Init_Timer0(void);										//��ʱ
void Init_Timer1(void);
void  InitLcd();														//��ʼ��lcd1602
void  lcd_printf(uchar *s,int temp_data);
void  WriteDataLCM(uchar dataW);									//LCD����
void  WriteCommandLCM(uchar CMD,uchar Attribc);				//LCDָ��
void  DisplayOneChar(uchar X,uchar Y,uchar DData);			//��ʾһ���ַ�
void  DisplayListChar(uchar X,uchar Y,uchar *DData,L);	//��ʾ�ַ���
void  InitMPU6050();													//��ʼ��MPU6050
void  Delay5us();
void  I2C_Start();
void  I2C_Stop();
void  I2C_SendACK(bit ack);
bit   I2C_RecvACK();
void  I2C_SendByte(uchar dat);
uchar I2C_RecvByte();
void  I2C_ReadPage();
void  I2C_WritePage();
void  display_ACCEL_x();
void  display_ACCEL_y();
void  display_ACCEL_z();
uchar Single_ReadI2C(uchar REG_Address);						//��ȡI2C����
void  Single_WriteI2C(uchar REG_Address,uchar REG_data);	//��I2Cд������
//****************************************
//����ת�ַ���
//****************************************
void lcd_printf(uchar *s,int temp_data)
{
	if(temp_data<0)
	{
		temp_data=-temp_data;
		*s='-';
	}
	else *s=' ';
	*++s =temp_data/100+0x30;
	temp_data=temp_data%100;     //ȡ������
	*++s =temp_data/10+0x30;
	temp_data=temp_data%10;      //ȡ������
	*++s =temp_data+0x30; 	
}
//****************************************
//��ʱ
//****************************************
void delay(unsigned int k)	
{						
	unsigned int i,j;				
	for(i=0;i<k;i++)
	{			
		for(j=0;j<121;j++);
	}						
}
/*------------------------------------------------
 uS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��,������ʱ
 �������� T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS��ʱ����������������� unsigned char t���޷���ֵ
 unsigned char �Ƕ����޷����ַ���������ֵ�ķ�Χ��
 0~255 ����ʹ�þ���12M����ȷ��ʱ��ʹ�û��
------------------------------------------------*/
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}
/*------------------------------------------------
                    ��ʱ����ʼ���ӳ���
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��		     
 TH0=0x0000;	      //0x00;������ֵ0x00;
 TL0=0x0000;
 ET0=1;           //��ʱ���жϴ�
 TR0=1;           //��ʱ�����ش�
 PT0=1;           //���ȼ���
}
/*------------------------------------------------
                    ��ʱ����ʼ���ӳ���
------------------------------------------------*/
void Init_Timer1(void)
{
 TMOD |= 0x10;	  //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ�� 
 TH1=0x00;	      //������ֵ������ʹ�ö�ʱ�����ֵ��0��ʼ����һֱ��65535���
 TL1=0x00;
 ET1=0;           //��ʱ���жϴ�
 TR1=0;           //��ʱ�����ش�
}


/*------------------------------------------------
��ʱ����ʼ���ӳ���
------------------------------------------------*/
/*void TIM2Inital(void)
{
  RCAP2H = (65536-2000)/256;//����12M 60ms 16bit �Զ�����
  RCAP2L = (65536-2000)%256;
  ET2=1;                     //�򿪶�ʱ���ж�
  TR2=1;                     //�򿪶�ʱ������
}*/
//****************************************
//LCD1602��ʼ��
//****************************************
void InitLcd()				
{			
	WriteCommandLCM(0x38,1);	
	WriteCommandLCM(0x08,1);	
	WriteCommandLCM(0x01,1);	
	WriteCommandLCM(0x06,1);	
	WriteCommandLCM(0x0c,1);
	DisplayOneChar(0,0,'A');
	DisplayOneChar(0,1,'G');
}			
//****************************************
//LCD1602д����
//****************************************
void WaitForEnable(void)	
{					
	DataPort=0xff;		
	LCM_RS=0;LCM_RW=1;_nop_();
	LCM_EN=1;_nop_();_nop_();
	while(DataPort&0x80);	
	LCM_EN=0;				
}					
//****************************************
//LCD1602д������
//****************************************
void WriteCommandLCM(uchar CMD,uchar Attribc)
{					
	if(Attribc)WaitForEnable();	
	LCM_RS=0;LCM_RW=0;_nop_();
	DataPort=CMD;_nop_();	
	LCM_EN=1;_nop_();_nop_();LCM_EN=0;
}					
//****************************************
//LCD1602д������
//****************************************
void WriteDataLCM(uchar dataW)
{					
	WaitForEnable();		
	LCM_RS=1;LCM_RW=0;_nop_();
	DataPort=dataW;_nop_();	
	LCM_EN=1;_nop_();_nop_();LCM_EN=0;
}		
//****************************************
//LCD1602д��һ���ַ�
//****************************************
void DisplayOneChar(uchar X,uchar Y,uchar DData)
{						
	Y&=1;						
	X&=15;						
	if(Y)X|=0x40;					
	X|=0x80;			
	WriteCommandLCM(X,0);		
	WriteDataLCM(DData);		
}						
//****************************************
//LCD1602��ʾ�ַ���
//****************************************
void DisplayListChar(uchar X,uchar Y,uchar *DData,L)
{
	uchar ListLength=0; 
	Y&=0x1;                
	X&=0xF;                
	while(L--)             
	{                       
		DisplayOneChar(X,Y,DData[ListLength]);
		ListLength++;  
		X++;                        
	}    
}
//**************************************
//��ʱ5΢��(STC90C52RC@12M)
//��ͬ�Ĺ�������,��Ҫ�����˺���
//������1T��MCUʱ,���������ʱ����
//**************************************
void Delay5us()
{
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}
//**************************************
//I2C��ʼ�ź�
//**************************************
void I2C_Start()
{
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 0;                    //�����½���
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
}
//**************************************
//I2Cֹͣ�ź�
//**************************************
void I2C_Stop()
{
    SDA = 0;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 1;                    //����������
    Delay5us();                 //��ʱ
}
//**************************************
//I2C����Ӧ���ź�
//��ڲ���:ack (0:ACK 1:NAK)
//**************************************
void I2C_SendACK(bit ack)
{
    SDA = ack;                  //дӦ���ź�
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ
}
//**************************************
//I2C����Ӧ���ź�
//**************************************
bit I2C_RecvACK()
{
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    CY = SDA;                   //��Ӧ���ź�
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ
    return CY;
}
//**************************************
//��I2C���߷���һ���ֽ�����
//**************************************
void I2C_SendByte(uchar dat)
{
    uchar i;
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;              //�Ƴ����ݵ����λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    I2C_RecvACK();
}
//**************************************
//��I2C���߽���һ���ֽ�����
//**************************************
uchar I2C_RecvByte()
{
    uchar i;
    uchar dat = 0;
    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    return dat;
}
//**************************************
//��I2C�豸д��һ���ֽ�����
//**************************************
void Single_WriteI2C(uchar REG_Address,uchar REG_data)
{
    I2C_Start();                  //��ʼ�ź�
    I2C_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    I2C_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
    I2C_SendByte(REG_data);       //�ڲ��Ĵ������ݣ�
    I2C_Stop();                   //����ֹͣ�ź�
}
//**************************************
//��I2C�豸��ȡһ���ֽ�����
//**************************************
uchar Single_ReadI2C(uchar REG_Address)
{
	uchar REG_data;
	I2C_Start();                   //��ʼ�ź�
	I2C_SendByte(SlaveAddress);    //�����豸��ַ+д�ź�
	I2C_SendByte(REG_Address);     //���ʹ洢��Ԫ��ַ����0��ʼ	
	I2C_Start();                   //��ʼ�ź�
	I2C_SendByte(SlaveAddress+1);  //�����豸��ַ+���ź�
	REG_data=I2C_RecvByte();       //�����Ĵ�������
	I2C_SendACK(1);                //����Ӧ���ź�
	I2C_Stop();                    //ֹͣ�ź�
	return REG_data;
}
//**************************************
//��ʼ��MPU6050
//**************************************
void InitMPU6050()
{
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//�������״̬
	Single_WriteI2C(SMPLRT_DIV, 0x07);
	Single_WriteI2C(CONFIG, 0x06);
	Single_WriteI2C(GYRO_CONFIG, 0x18);
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}
//**************************************
//�ϳ�����
//**************************************
int GetData(uchar REG_Address)
{
	char H,L;
	H=Single_ReadI2C(REG_Address);
	L=Single_ReadI2C(REG_Address+1);
	return (H<<8)+L;   //�ϳ�����
}
//**************************************
//��1602����ʾ10λ����
//**************************************
void Display10BitData(int value,uchar x,uchar y)
{
	value/=64;							//ת��Ϊ10λ����
	lcd_printf(dis, value);			//ת��������ʾ
	DisplayListChar(x,y,dis,4);	//��ʼ�У��У���ʾ���飬��ʾ����
}
//**************************************
//��ʾ�¶�
//**************************************
/*------------------------------------------------
                 ��ʱ���ж��ӳ���
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{ 
if(maichong==0)  // ռ�ձ�
{
 TH0 =tun0;	      //0xFFB0������ֵ������ʹ�ö�ʱ�����ֵ��0��ʼ����һֱ��65535���
 TL0 = tun1;
  }

else 
{
TH0 = tun3;	      //0xFFB0������ֵ������ʹ�ö�ʱ�����ֵ��0��ʼ����һֱ��65535���
 TL0 = tun4;
 times2=0;
}
maichong=~maichong;

}
/*------------------------------------------------
                 ��ʱ���ж��ӳ���
------------------------------------------------*/
void Timer1_isr(void) interrupt 3 using 1
{
 TH1=0x00;		 //���¸�ֵ
 TL1=0x00;
 times1=0;
}
/*------------------------------------------------
                 ��ʱ���ж��ӳ���
------------------------------------------------*/
/*void TIM2(void) interrupt 5 using 2//��ʱ��2�ж�
{
  TF2=0;

  }
*/
//*********************************************************
//������
//*********************************************************
void main()
{ 
	delay(500);		//�ϵ���ʱ		
          EA=1;            //���жϹ�
          //ET1=1;           //��ʱ���жϴ�
          //ET2=1;                     //�򿪶�ʱ���ж�
	//InitLcd();		//Һ����ʼ��
	InitMPU6050();	//��ʼ��MPU6050
          Init_Timer0();
	//Init_Timer1();
          //EA=1;
	delay(150); 

	while(1)
	{
/* if(times1==1)
   {
 		Display10BitData(GetData(ACCEL_XOUT_H),2,0);	//��ʾX����ٶ�
		Display10BitData(GetData(ACCEL_YOUT_H),7,0);	//��ʾY����ٶ�
		Display10BitData(GetData(ACCEL_ZOUT_H),12,0);	//��ʾZ����ٶ�
		Display10BitData(GetData(GYRO_XOUT_H),2,1);	//��ʾX����ٶ�
		Display10BitData(GetData(GYRO_YOUT_H),7,1);	//��ʾY����ٶ�
		Display10BitData(GetData(GYRO_ZOUT_H),12,1);	//��ʾZ����ٶ�
		times1=0;//		delay(500);
   }*/

if(times2==0)
{
Xjiaodu=GetData(ACCEL_XOUT_H);//240;//
times2=1;
if(Xjiaodu<0)
{
Xjiaodu=-Xjiaodu;
dir=1;
}
else
{
dir=0;
}
Xjiaodu=Xjiaodu/16;






Ki=T/Ti;  
Kd=Td/T;   //��Ҫȷ��Kp,Ti,Td;
Kp=Kp;
KI=Kp*Ki;
KD=Kp*Kd;
//time2=4*Xjiaodu+280;//������
error=Xjiaodu-E0;//���Ƕ�֮��
if(abs(error)>E0)
{
   P_now=Kp*(error-error_1)+/*KI*error+KD*(error-2*error_1+error_2)*/+P_last1;
}
else
{
   P_now=Kp*(error-error_1)+KI*error/*KD*(error-2*error_1+error_2)+P_last1*/;
}
error_1=error;
error_2=error_1;
P_last1=P_now;
time2=P_now;

if(time2>900) //15-45
{
  time2=900;
}

tun0=(65535-time2)>>8;  //%256    ��1000HZ    �ߵ�ƽ ��ʱtime2
tun1=(65535-time2)&0xFF;//(/256)

tun3=(64535+time2)>>8;  //�͵�ƽ ��ʱ 1000-time2
tun4=(64535+time2)&0xFF;

}

}

}
