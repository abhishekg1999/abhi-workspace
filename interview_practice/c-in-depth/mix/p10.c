#include <stdlib.h>
#include <stdio.h>
int* func(void);
int main()
{
 const int *ptr=func();
 *ptr=7;
 printf("*ptr=%d",*ptr);
 return 0;
  
}

int * func(void)
{
  int *p=malloc(sizeof(int));
  return 0;
}
