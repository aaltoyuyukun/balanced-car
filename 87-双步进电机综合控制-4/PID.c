#include<math.h>
#include<reg52.h>
#include<stdio.h>
void main(void)
{
 int E0=0; //设定目标Desired Value 周期T先设为1ms
 int error,error_1,error_2; //Error[-1]
 double Kp,KI,KD; //比例常数Proportional Const
 double Ki,Kd,T,V1,V2,P_now,P_last1,P_last2;
 double Ti,Td;//自行定义Kp,Ti,Td;
 
V1=5;
V2=10;
Ki=T/Ti;  
Kd=Td/T;   //主要确定Kp,Ti,Td;
error=V1-V2;
error_1=0;
error_2=0;
P_last1=P_last2=0;
Kp=Kp;KI=Kp*Ki;KD=Kp*Kd;
if(abs(error)>E0)
{
  P_now=Kp*(error-error_1)+KD*(error-2*error_1+error_2)+P_last1;
}
else
{
   P_now=Kp*(error-error_1)+KI*error+KD*(error-2*error_1+error_2)+P_last1;
}
error_1=error;
error_2=error_1;
P_last1=P_now;
printf("%lf",P_now);
}
