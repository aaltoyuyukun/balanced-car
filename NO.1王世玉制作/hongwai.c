#include <reg52.h>
#include "hongwai.h"

void Ircordpro(void)//������ֵ������
{ 
  unsigned char i, j, k;
  unsigned char cord,value;

  k=1;
  for(i=0;i<4;i++)      //����4���ֽ�
     {
      for(j=1;j<=8;j++) //����1���ֽ�8λ
         {
          cord=irdata[k];
          if(cord>7)//����ĳֵΪ1������;����о��Թ�ϵ������ʹ��12M���㣬��ֵ������һ�����
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
	 irpro_ok=1;//������ϱ�־λ��1
}

unsigned char Ir_work(void)//�����ֵɢת����
{
       switch(IRcord[2])//�жϵ���������ֵ
	         {
			 case 0x0c:return 1;break;//1 ��ʾ��Ӧ�İ���ֵ
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

		  irpro_ok=0;//������ɱ�־

  }