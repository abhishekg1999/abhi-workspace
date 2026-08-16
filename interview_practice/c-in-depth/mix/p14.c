#include <stdlib.h>
#include <stdio.h>
int thrice (int i);
int main()
{
 const int i=23;
 const int j=thrice(i);
 printf("j=%d\n",j);
  
}

int thrice(int i)
{
  return 3*i;
}
