//****************************************
// Update to MPU6050 by shinetop
// MCU: STC89C52
// 2012.3.1
// 功能: 显示加速度计和陀螺仪的10位原始数据
//****************************************
// GY-52 MPU3050 IIC测试程序
// 使用单片机STC89C51 
// 晶振：11.0592M
// 显示：LCD1602
// 编译环境 Keil uVision2
// 参考宏晶网站24c04通信程序
// 时间：2011年9月1日
// QQ：531389319
//****************************************
#include <REG52.H>	
#include <math.h>    //Keil library  
#include <stdio.h>   //Keil library	
#include <INTRINS.H>
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
//****************************************
// 定义51单片机端口
//****************************************
#define DataPort P0		//LCD1602数据端口
sbit    SCL=P1^0;			//IIC时钟引脚定义
sbit    SDA=P1^1;			//IIC数据引脚定义
sbit    LCM_RS=P2^4;		//LCD1602命令端口		
sbit    LCM_RW=P2^5;		//LCD1602命令端口		
sbit    LCM_EN=P2^6;		//LCD1602命令端口 
sbit maichong=P1^2; //定义大功率步进电机连接端口
sbit dir=P1^3;
//****************************************
// 定义MPU6050内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
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
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I			0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0	//IIC写入时的地址字节数据，+1为读取
//****************************************
//定义类型及变量
//****************************************
uchar dis[4];							//显示数字(-511至512)的字符数组
unsigned int speed;
int	dis_data;	
static unsigned int  Kp=10,KI,KD,Ti=10,Td=5;//自行定义Kp,Ti,Td;; //比例常数Proportional Const
double  Ki,Kd,T=0.001; 
double error=0,error_1=0,error_2=0; //Error[-1]
double E0=23,E1=0,E2=0; //设定目标Desired Value 周期T先设为1ms					//变量
double P_now=0,P_last1=0,P_last2=0;
double Xjiaodu;
unsigned char num;
unsigned int time2=0,times1=0,times2=0;
static unsigned int tun0=0,tun1=0,tun3=0,tun4=0;
//****************************************
//函数声明
//****************************************
void  delay(unsigned int k);
void Init_Timer0(void);										//延时
void Init_Timer1(void);
void  InitLcd();														//初始化lcd1602
void  lcd_printf(uchar *s,int temp_data);
void  WriteDataLCM(uchar dataW);									//LCD数据
void  WriteCommandLCM(uchar CMD,uchar Attribc);				//LCD指令
void  DisplayOneChar(uchar X,uchar Y,uchar DData);			//显示一个字符
void  DisplayListChar(uchar X,uchar Y,uchar *DData,L);	//显示字符串
void  InitMPU6050();													//初始化MPU6050
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
uchar Single_ReadI2C(uchar REG_Address);						//读取I2C数据
void  Single_WriteI2C(uchar REG_Address,uchar REG_data);	//向I2C写入数据
//****************************************
//整数转字符串
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
	temp_data=temp_data%100;     //取余运算
	*++s =temp_data/10+0x30;
	temp_data=temp_data%10;      //取余运算
	*++s =temp_data+0x30; 	
}
//****************************************
//延时
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
 uS延时函数，含有输入参数 unsigned char t，无返回值
 unsigned char 是定义无符号字符变量，其值的范围是
 0~255 这里使用晶振12M，精确延时请使用汇编,大致延时
 长度如下 T=tx2+5 uS 
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS延时函数，含有输入参数 unsigned char t，无返回值
 unsigned char 是定义无符号字符变量，其值的范围是
 0~255 这里使用晶振12M，精确延时请使用汇编
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
                    定时器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 TH0=0x0000;	      //0x00;给定初值0x00;
 TL0=0x0000;
 ET0=1;           //定时器中断打开
 TR0=1;           //定时器开关打开
 PT0=1;           //优先级打开
}
/*------------------------------------------------
                    定时器初始化子程序
------------------------------------------------*/
void Init_Timer1(void)
{
 TMOD |= 0x10;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响 
 TH1=0x00;	      //给定初值，这里使用定时器最大值从0开始计数一直到65535溢出
 TL1=0x00;
 ET1=0;           //定时器中断打开
 TR1=0;           //定时器开关打开
}


/*------------------------------------------------
定时器初始化子程序
------------------------------------------------*/
/*void TIM2Inital(void)
{
  RCAP2H = (65536-2000)/256;//晶振12M 60ms 16bit 自动重载
  RCAP2L = (65536-2000)%256;
  ET2=1;                     //打开定时器中断
  TR2=1;                     //打开定时器开关
}*/
//****************************************
//LCD1602初始化
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
//LCD1602写允许
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
//LCD1602写入命令
//****************************************
void WriteCommandLCM(uchar CMD,uchar Attribc)
{					
	if(Attribc)WaitForEnable();	
	LCM_RS=0;LCM_RW=0;_nop_();
	DataPort=CMD;_nop_();	
	LCM_EN=1;_nop_();_nop_();LCM_EN=0;
}					
//****************************************
//LCD1602写入数据
//****************************************
void WriteDataLCM(uchar dataW)
{					
	WaitForEnable();		
	LCM_RS=1;LCM_RW=0;_nop_();
	DataPort=dataW;_nop_();	
	LCM_EN=1;_nop_();_nop_();LCM_EN=0;
}		
//****************************************
//LCD1602写入一个字符
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
//LCD1602显示字符串
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
//延时5微秒(STC90C52RC@12M)
//不同的工作环境,需要调整此函数
//当改用1T的MCU时,请调整此延时函数
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
//I2C起始信号
//**************************************
void I2C_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}
//**************************************
//I2C停止信号
//**************************************
void I2C_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}
//**************************************
//I2C发送应答信号
//入口参数:ack (0:ACK 1:NAK)
//**************************************
void I2C_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}
//**************************************
//I2C接收应答信号
//**************************************
bit I2C_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
    return CY;
}
//**************************************
//向I2C总线发送一个字节数据
//**************************************
void I2C_SendByte(uchar dat)
{
    uchar i;
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    I2C_RecvACK();
}
//**************************************
//从I2C总线接收一个字节数据
//**************************************
uchar I2C_RecvByte()
{
    uchar i;
    uchar dat = 0;
    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}
//**************************************
//向I2C设备写入一个字节数据
//**************************************
void Single_WriteI2C(uchar REG_Address,uchar REG_data)
{
    I2C_Start();                  //起始信号
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号
    I2C_SendByte(REG_Address);    //内部寄存器地址，
    I2C_SendByte(REG_data);       //内部寄存器数据，
    I2C_Stop();                   //发送停止信号
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
uchar Single_ReadI2C(uchar REG_Address)
{
	uchar REG_data;
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress);    //发送设备地址+写信号
	I2C_SendByte(REG_Address);     //发送存储单元地址，从0开始	
	I2C_Start();                   //起始信号
	I2C_SendByte(SlaveAddress+1);  //发送设备地址+读信号
	REG_data=I2C_RecvByte();       //读出寄存器数据
	I2C_SendACK(1);                //接收应答信号
	I2C_Stop();                    //停止信号
	return REG_data;
}
//**************************************
//初始化MPU6050
//**************************************
void InitMPU6050()
{
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//解除休眠状态
	Single_WriteI2C(SMPLRT_DIV, 0x07);
	Single_WriteI2C(CONFIG, 0x06);
	Single_WriteI2C(GYRO_CONFIG, 0x18);
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}
//**************************************
//合成数据
//**************************************
int GetData(uchar REG_Address)
{
	char H,L;
	H=Single_ReadI2C(REG_Address);
	L=Single_ReadI2C(REG_Address+1);
	return (H<<8)+L;   //合成数据
}
//**************************************
//在1602上显示10位数据
//**************************************
void Display10BitData(int value,uchar x,uchar y)
{
	value/=64;							//转换为10位数据
	lcd_printf(dis, value);			//转换数据显示
	DisplayListChar(x,y,dis,4);	//启始列，行，显示数组，显示长度
}
//**************************************
//显示温度
//**************************************
/*------------------------------------------------
                 定时器中断子程序
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{ 
if(maichong==0)  // 占空比
{
 TH0 =tun0;	      //0xFFB0给定初值，这里使用定时器最大值从0开始计数一直到65535溢出
 TL0 = tun1;
  }

else 
{
TH0 = tun3;	      //0xFFB0给定初值，这里使用定时器最大值从0开始计数一直到65535溢出
 TL0 = tun4;
 times2=0;
}
maichong=~maichong;

}
/*------------------------------------------------
                 定时器中断子程序
------------------------------------------------*/
void Timer1_isr(void) interrupt 3 using 1
{
 TH1=0x00;		 //重新赋值
 TL1=0x00;
 times1=0;
}
/*------------------------------------------------
                 定时器中断子程序
------------------------------------------------*/
/*void TIM2(void) interrupt 5 using 2//定时器2中断
{
  TF2=0;

  }
*/
//*********************************************************
//主程序
//*********************************************************
void main()
{ 
	delay(500);		//上电延时		
          EA=1;            //总中断关
          //ET1=1;           //定时器中断打开
          //ET2=1;                     //打开定时器中断
	//InitLcd();		//液晶初始化
	InitMPU6050();	//初始化MPU6050
          Init_Timer0();
	//Init_Timer1();
          //EA=1;
	delay(150); 

	while(1)
	{
/* if(times1==1)
   {
 		Display10BitData(GetData(ACCEL_XOUT_H),2,0);	//显示X轴加速度
		Display10BitData(GetData(ACCEL_YOUT_H),7,0);	//显示Y轴加速度
		Display10BitData(GetData(ACCEL_ZOUT_H),12,0);	//显示Z轴加速度
		Display10BitData(GetData(GYRO_XOUT_H),2,1);	//显示X轴角速度
		Display10BitData(GetData(GYRO_YOUT_H),7,1);	//显示Y轴角速度
		Display10BitData(GetData(GYRO_ZOUT_H),12,1);	//显示Z轴角速度
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
Kd=Td/T;   //主要确定Kp,Ti,Td;
Kp=Kp;
KI=Kp*Ki;
KD=Kp*Kd;
//time2=4*Xjiaodu+280;//给定量
error=Xjiaodu-E0;//两角度之差
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

tun0=(65535-time2)>>8;  //%256    总1000HZ    高电平 延时time2
tun1=(65535-time2)&0xFF;//(/256)

tun3=(64535+time2)>>8;  //低电平 延时 1000-time2
tun4=(64535+time2)&0xFF;

}

}

}
