#include <stdio.h>
static int global=10;
void main()
{
  static int local=20;
  printf("local= %d global= %d\n",local,global);
 }
