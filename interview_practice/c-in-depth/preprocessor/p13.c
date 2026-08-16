#include <stdlib.h>
#include <stdio.h>
#define INT int
int main()
{
  INT a=2,*p=&a;
  printf("%d %d\n",a,*p);
  return 0;
  
}
