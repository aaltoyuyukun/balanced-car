#include <reg52.h>
#include "bujindianji.h"

void bujin(bit on,bit lifrit)
{
  if(on)
  {
   	if(lifrit)
   	{
   	if(ba>7)
    	ba=0;
   	if(bb>7)
    	bb=0;	   

   	if(bc>7)
    	bc=0;
   	if(bd>7)
    	bd=0;
   	A1=BJ[ba];
   	B1=BJ[bb];
   	C1=BJ[bc];
   	D1=BJ[bd];
   	ba++;bb++;bc++;bd++;
    }
	else
	 {
   	  A1=BJ[ba];
   	  B1=BJ[bb];
   	  C1=BJ[bc];
   	  D1=BJ[bd];
   	if(ba==0)
    	ba=8;
   	if(bb==0)
    	bb=8;
   	if(bc==0)
    	bc=8;
   	if(bd==0)
    	bd=8;	 
  ba--;bb--;bc--;bd--;
	 }

  }
 else
   {
   A1=0;
   B1=0;
   C1=0;
   D1=0;
   }
}