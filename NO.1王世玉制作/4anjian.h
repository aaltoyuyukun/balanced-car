/*-----------------------------------------------
  ���ƣ�4anjian.h
  ��д��wangshiyu
  ���ڣ�2013.11.25
  �޸ģ���д
  ���ݣ�
  ���Ŷ������£�UP P3^7, DOWN P3^6��LIFT P3^5��RIGHT P3^4. 
------------------------------------------------*/




#include<reg52.h> //����ͷ�ļ�

#ifndef __4ANJIAN_H__
#define __4ANJIAN_H__
bit an1=0,an2=0,an3=0,an4=0;
sbit UP=P3^7;
sbit DOWN=P3^6;
sbit LIFT=P3^5;
sbit RIGHT=P3^4;
//unsigned char anflog,n;
unsigned char up,dow,li,ri;	   
unsigned char KeyScan(void);



#endif