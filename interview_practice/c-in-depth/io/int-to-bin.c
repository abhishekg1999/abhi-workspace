#include <stdio.h>
int main()
{

  int i=-1 ,j=1,pos;  
  for(pos=31; pos>=0; pos--)
  printf("%d",i>>pos&1);

  printf("\n");
  for(pos=31; pos>=0; pos--)
  printf("%d",j>>pos&1);
  printf("\n");

  printf("%d\n",j>i);
 }
