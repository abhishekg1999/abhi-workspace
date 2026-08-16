#include <stdio.h>
int main()
{
  char first[5],sec[5];
  printf("enter sec string\n");
  scanf("%s",sec);
  printf("sec--> %s\n",sec);

  printf("enter first string\n");
  scanf("%s",first);

  printf("first->%s sec->%s ",first,sec);
}
