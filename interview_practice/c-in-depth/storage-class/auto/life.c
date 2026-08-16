#include <stdio.h>
int* fun1(void);
void main()
{
  int *p;
  p=fun1();
  printf("%d\n",*p); //Segmentation fault (core dumped) 
  *p=30;
  printf("%d\n",*p);
}

 int* fun1(void)
 {
   int var=10;
   return &var;
 }
