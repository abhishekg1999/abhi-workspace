#include <stdio.h>
int main()
{
  int i;
  char s[]= "abcd";
  printf("%s\n",s);
  for (i=0;s[i];i++);
  for(i=i-1;i>=0; i--)
  printf("%c",s[i]);
  printf("\n");
  }
