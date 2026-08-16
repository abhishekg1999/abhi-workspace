#include <stdio.h>
static int var=10;
void main()
{
  static int var=20;
  printf("%d\n",var);
 }
