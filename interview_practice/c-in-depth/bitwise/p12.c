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
  unsigned int i,r=0;
  for(i=0;num!=0;i++)
  {
  	r=(r<<1)|num&1;
	num>>=1;
  }
  r<<=32-i;
  return r;
}
