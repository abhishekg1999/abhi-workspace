#include <stdlib.h>
#include <stdio.h>
void func(struct {int i;char c;}v);
int main()
{
  struct {int i; char c;}var={2,'s'};
  func(var);
  return 0;
  
}

void func(struct {int i; char c;}v)
{  

   printf("%d %c\n",v.i,v.c);
 }
