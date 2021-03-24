#include <reg52.h>
#include"4anjian.h"
#include"laba.h"
///////按键外加按键音//////定义端口在laba.h

//中断调用按键扫描程序，4个独立按键，按键函数分为短按长按，具有返回值。
//本程序短按一个返回值，长按一个返回值。
//移植使用时请根据自身情况
unsigned char KeyScan(void)										   
{

if(UP==0)
 {	 
  up++; 
 if(up==3)   
    an1=1;
 if(up==10)
   {
   an1=0;  
   up=4;
   return 11;
   }     	
 } 
 else
  {
  up=0;
  if(an1==1)
    {
	an1=0;
	return 1;
	} 
  }
  
  
if(DOWN==0)
 {
 dow++; 
 if(dow==3)	
	an2=1;
 if(dow==10)
   {
    an2=0;
    dow=4;
	return 22;
   } 	
 }
else
  dow=0;
  if(an2==1)
    {
	an2=0;
	return 2;
	} 
   
if(LIFT==0)
 {
 li++;  
 if(li==3)
   	an3=1;	  
   if(li==10)
    {
	an3=0;
	li=4;
	return 33;
	}	
 }
else
   li=0;
   if(an3==1)
    {
	an3=0;
	return 3;
	} 


if(RIGHT==0)
 {
  ri++;  
  if(ri==3)  
	an4=1;
  if(ri==10)
    {
   	an4=0;
    ri=4;
	return 44;
	}	
 }
else
  ri=0;
  if(an4==1)
    {
	an4=0;
	return 4;
	}    
}