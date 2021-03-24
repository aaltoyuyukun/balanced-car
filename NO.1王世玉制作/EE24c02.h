#include<reg52.h> //包含头文件
#include <intrins.h>

#ifndef __EE24C02_H__
#define __EE24C02_H__


#define  _Nop()  _nop_()        //定义空指令
sbit SDA=P2^1;            //模拟I2C数据传送位
sbit SCL=P2^0;            //模拟I2C时钟控制位                          
bit ack;	              //应答标志位

void DelayUs2x(unsigned char t);//函数声明 
void DelayMs(unsigned char t);
void Start_I2c();
void Stop_I2c();
void  SendByte(unsigned char c);
unsigned char  RcvByte();
void Ack_I2c(void);
void NoAck_I2c(void);
bit ISendStr(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no);
bit IRcvStr(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no);

#endif
