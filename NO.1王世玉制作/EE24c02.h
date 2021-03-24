#include<reg52.h> //����ͷ�ļ�
#include <intrins.h>

#ifndef __EE24C02_H__
#define __EE24C02_H__


#define  _Nop()  _nop_()        //�����ָ��
sbit SDA=P2^1;            //ģ��I2C���ݴ���λ
sbit SCL=P2^0;            //ģ��I2Cʱ�ӿ���λ                          
bit ack;	              //Ӧ���־λ

void DelayUs2x(unsigned char t);//�������� 
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
