#include<reg52.h> //包含头文件
#ifndef __HONGWAI_H__
#define __HONGWAI_H__




sbit IR=P3^2;  //红外接口标志
bit irpro_ok,irok;
unsigned char IRcord[4];
unsigned char irdata[33];

unsigned char Ir_work(void);//红外键值散转程序
void Ircordpro(void);

#endif
