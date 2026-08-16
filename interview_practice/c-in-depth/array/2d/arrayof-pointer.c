#include <stdio.h>
void main()
{
  int i=10,j=20,k=30;
  int *p[3];

  p[0]=&i;
  p[1]=&j;
  p[2]=&k;

  printf("%d %d %d\n",*p[0],*p[1],*p[2]);
  printf("%p %p %p\n",p[0],p[1],p[2]);
 }
