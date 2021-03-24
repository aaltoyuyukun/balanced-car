/*-----------------------------------------------
  名称：LCD1602
  编写：wangshiyu
  日期：2013.11.25
  修改：无
  内容：
  引脚定义如下：1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
------------------------------------------------*/
#include "1602.h"
#include <reg52.h>



/*------------------------------------------------
              判忙函数
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
              写入命令函数
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //忙则等待
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 

 EN_CLR;
 }
/*------------------------------------------------
              写入数据函数
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //忙则等待
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 

 EN_CLR;
 }

/*------------------------------------------------
                清屏函数
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              写入字符串函数
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
              写入字符函数
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
              初始化函数
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*显示模式设置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*显示关闭*/ 
   LCD_Write_Com(0x01);    /*显示清屏*/ 
   LCD_Write_Com(0x06);    /*显示光标移动设置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*显示开及光标设置*/
   }
   
 void Show(unsigned char k,unsigned char a,unsigned char dat)
{
    unsigned char i, j=0;
    unsigned char buf[6];
  	unsigned char vcode[]={'0','1','2','3','4','5','6','7','8','9'};
    // 整形数转换为BCD码 
    i = 0;
    do {
        buf[i] = dat%10;
        dat /= 10;
        i++;
        if (i >= 6) {
            break;
        }
    } while (dat>0);

    
    //显示在1602上 /
	   
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