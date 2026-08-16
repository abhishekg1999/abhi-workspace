#include <stdlib.h>
#include <stdio.h>
void func(void);
int x=2;
static int y=5;
int main()
{
 int x=3;
 func();
 func();
 printf("inside main x=%d y=%d\n",x,y);
 return 0;
  
}

void func(void)
{
   static int x;
   x=x+2;
   printf("inside func x=%d y=%d\n",x,y);
 }
