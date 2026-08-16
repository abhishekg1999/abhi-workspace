#include<stdio.h>
int main()
{

  int i=10,j=2,k;
  float f=23.5;
  k=i<<j;
  printf("i<<j=%d\n",k);

  printf("%d\n",i>>j);

  k=printf("helloworld\n");

  printf("%d\n",k);

  k=printf("k=%f\n",f);
  printf("k=%d\n",k);
  }
