#include <stdlib.h>
#include <stdio.h>
int main()
{
 int i,sum=0;
 for(i=0;i<5;i++)
 {
   static int i=10;
   sum=sum + i++;
  }

  printf("sum=%d\n",sum);
  return 0;
  
}
