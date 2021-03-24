/*-----------------------------------------------
  名称：bujindianji.h
  编写：wangshiyu
  日期：2013.11.25
  修改：编写
  内容：
  引脚定义如下： 
------------------------------------------------*/




#include<reg52.h> //包含头文件

#ifndef __BUJINDIANJI_H__
#define __BUJINDIANJI_H__

sbit A1=P1^1; //定义步进电机连接端口
sbit B1=P1^2;
sbit C1=P1^3;
sbit D1=P1^4;

unsigned char code BJ[8]={1,1,1,0,0,0,0,0};
unsigned char ba=0,bb=6,bc=4,bd=2;	



#endif