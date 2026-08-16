#include <stdio.h>
//register int m;  //error: register name not specified for ‘m’
void main()
{
  register int i;
  printf("register= %d\n",i);
}
