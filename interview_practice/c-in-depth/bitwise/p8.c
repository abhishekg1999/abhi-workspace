#include <stdlib.h>
#include <stdio.h>
int main()
{
  int i,num=0xA0DF;
  for(i=31;i>=0;i--)
  	printf("%d",(num>>i)&1);

  return 0;
  
}
