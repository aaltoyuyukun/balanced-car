#include<reg52.h> //����ͷ�ļ�
#ifndef __HONGWAI_H__
#define __HONGWAI_H__




sbit IR=P3^2;  //����ӿڱ�־
bit irpro_ok,irok;
unsigned char IRcord[4];
unsigned char irdata[33];

unsigned char Ir_work(void);//�����ֵɢת����
void Ircordpro(void);

#endif
