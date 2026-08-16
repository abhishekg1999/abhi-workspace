#include <stdio.h>
int main()
{
 int a=5,*ptr;
 ptr=&a;
 printf("input a number:");
 scanf("%d",ptr);
 printf("%d %d\n",a,*ptr);
 return 0;
  
}
