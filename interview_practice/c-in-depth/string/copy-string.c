#include <stdio.h>
int main()
{
  int i;
  char s[10]="abcd",d[10];
  printf("********befor******\n");
  printf("%s %s\n",s,d);

  for(i=0;s[i];i++)
  d[i]=s[i];

  d[i]=s[i];
  printf("******after*******\n");
  printf("%s %s\n",s,d);
 }
