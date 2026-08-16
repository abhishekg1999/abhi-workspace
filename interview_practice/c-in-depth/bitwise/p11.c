#include <stdlib.h>
#include <stdio.h>
int func(unsigned int);
int main()
{
 printf("%d\n",func(0x1AE3));
 return 0;
  
}

int func(unsigned int x)
{
  int count=0,mask=1,i;
  for(i=0;i<32;i++)
  {
  	if((x&mask)!=0)
			count++;
	mask<<=1;
   }
   return count;
 }
