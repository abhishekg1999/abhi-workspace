#include <stdio.h>
void main()
{
  int a[5]={10,20,30,40,50};
  int *ip;
  int (*p)[3];
  ip=a;
  p=a;
  printf("ip= %u p= %u\n",ip,p);
  //printf("*ip= %d *p= %d\n",*ip,*p);
  printf("(*p)[0]= %d (*p)[1]= %d (*p)[2]= %d\n",(*p)[0],(*p)[1],(*p)[2]);
  printf("p[0]= %u p[1]= %u p[2]= %u\n",p[0],p[1],p[2]);
  printf("p[0]= %u p[1]= %u p[2]= %u\n",*p,*p+1,*p+2);
  }
