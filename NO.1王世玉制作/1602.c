/*-----------------------------------------------
  ���ƣ�LCD1602
  ��д��wangshiyu
  ���ڣ�2013.11.25
  �޸ģ���
  ���ݣ�
  ���Ŷ������£�1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
------------------------------------------------*/
#include "1602.h"
#include <reg52.h>



/*------------------------------------------------
              ��æ����
------------------------------------------------*/
 bit LCD_Check_Busy(void) 
 { 
#ifdef CHECK_BUSY
 DataPort= 0xFF; 
 RS_CLR; 
 RW_SET; 
 EN_CLR; 
 
 EN_SET;
 return (bit)(DataPort & 0x80);
#else
 return 0;
#endif
 }
/*------------------------------------------------
              д�������
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 

 EN_CLR;
 }
/*------------------------------------------------
              д�����ݺ���
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 

 EN_CLR;
 }

/*------------------------------------------------
                ��������
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              д���ַ�������
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
       
 while (*s) 
 	{     
 LCD_Write_Char(x,y,*s);     
 s ++;  x++;   
 	}
 }
/*------------------------------------------------
              д���ַ�����
------------------------------------------------*/
void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);     
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);     
 	}        
 LCD_Write_Data( Data);  
 }
/*------------------------------------------------
              ��ʼ������
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*��ʾģʽ����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*��ʾ�ر�*/ 
   LCD_Write_Com(0x01);    /*��ʾ����*/ 
   LCD_Write_Com(0x06);    /*��ʾ����ƶ�����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*��ʾ�����������*/
   }
   
 void Show(unsigned char k,unsigned char a,unsigned char dat)
{
    unsigned char i, j=0;
    unsigned char buf[6];
  	unsigned char vcode[]={'0','1','2','3','4','5','6','7','8','9'};
    // ������ת��ΪBCD�� 
    i = 0;
    do {
        buf[i] = dat%10;
        dat /= 10;
        i++;
        if (i >= 6) {
            break;
        }
    } while (dat>0);

    
    //��ʾ��1602�� /
	   
	  for (j;j<i;j++) 
    	{
	   LCD_Write_Char(k,a,vcode[buf[i-j-1]]);
	   k++;
	    }
    
}

 void DelayMs(unsigned char i)
  {
  while (i--);
  }