#include <stdio.h>
int func(void);
int main()
{
  int x=10;
  x=func();
  printf("x=%d\n",x);
  return 0;
  
}

int func(void)
{
  printf("functions\n");

 }
