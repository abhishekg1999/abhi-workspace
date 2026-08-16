#include <stdio.h>
int main()
{
  int i,j;
  char s[10]= "hello";
  char d[10];
  for(i=0;s[i];i++);
  for(i=i-1,j=0;i>=0;j++,i--)
  d[j]=s[i];

  d[j]='\0';

  printf("%s %s\n",s,d);
}
