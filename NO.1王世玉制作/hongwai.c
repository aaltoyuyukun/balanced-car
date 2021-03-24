#include <reg52.h>
#include "hongwai.h"

void Ircordpro(void)//红外码值处理函数
{ 
  unsigned char i, j, k;
  unsigned char cord,value;

  k=1;
  for(i=0;i<4;i++)      //处理4个字节
     {
      for(j=1;j<=8;j++) //处理1个字节8位
         {
          cord=irdata[k];
          if(cord>7)//大于某值为1，这个和晶振有绝对关系，这里使用12M计算，此值可以有一定误差
             value|=0x80;
          if(j<8)
		    {
			 value>>=1;
			}
           k++;
         }
     IRcord[i]=value;
     value=0;     
     } 
	 irpro_ok=1;//处理完毕标志位置1
}

unsigned char Ir_work(void)//红外键值散转程序
{
       switch(IRcord[2])//判断第三个数码值
	         {
			 case 0x0c:return 1;break;//1 显示相应的按键值
			 case 0x18:return 1;break;//2
			 case 0x5e:return 1;break;//3
			 case 0x08:return 1;break;//4
			 case 0x1c:return 1;break;//5
			 case 0x5a:return 1;break;//6
			 case 0x42:return 1;break;//7
			 case 0x52:return 1;break;//8
			 case 0x4a:return 1;break;//9
             default:break;
			 }

		  irpro_ok=0;//处理完成标志

  }