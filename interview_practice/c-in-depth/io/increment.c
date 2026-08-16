#include <stdio.h>
int main()
{
  int i=0,j=10,k;
  printf("%d\n",i);
  ++i;
  printf("%d\n",i);

  k=j++;
  printf("%d %d\n",j,k);
}
