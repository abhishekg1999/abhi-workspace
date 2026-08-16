#include <stdlib.h>
#include <stdio.h>
int main()
{
 unsigned int num=0x1F,pos=3,bit;
 bit=(num>>pos)&1;
 printf("%u\n",bit);
 return 0;
  
}
