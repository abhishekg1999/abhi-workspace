#include <stdio.h>
void func(int a,static int b);
int main()
{
 func(1,2);
 func(3,4);
 return 0;

  
}

void func(int a,static int b) //cant pass storage class in formal args
{
  a++;
  b++;
  printf("%d %d\n",a,b);
 }
