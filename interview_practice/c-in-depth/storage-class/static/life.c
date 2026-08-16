#include <stdio.h>
int* fun1(void);
void main()
{
  int *p;
  p=fun1();
  *p=30;
  printf("%d\n",*p);
 }

 int* fun1(void)
 {
   static int fun1_var=20;
   return &fun1_var;
  }
