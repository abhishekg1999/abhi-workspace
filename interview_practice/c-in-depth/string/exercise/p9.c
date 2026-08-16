#include <string.h>
#include <stdio.h>
int main()
{
 char str[]="painstacking";
 char *p=str+5;
 printf("%c\t",*p);
 printf("%s\n",p);
 return 0;
}
