#include <stdlib.h>
#include <stdio.h>
unsigned int func(unsigned int num);
int main()
{
 unsigned int x=0x1AE3;
 displayBits(x);
 x=func(x);
 printf("%X\n",x);
 displayBits(x);
 return 0;
  
}

unsigned int func(unsigned int num)
{
  unsigned int lmask,rmask,mask;
  lmask=1<<31;
  rmask=1;
  while(lmask>rmask)
  {
  	mask=lmask|rmask;
	if((num&mask)!=0 && (num&mask)!=mask)
		num^=mask;
	 lmask>>=1;
	 rmask<<=1;
  }
  return num;
}
