//this is the example of segmentation fault //
#include <string.h>
#include <stdio.h>
int main()
{
 char *str;
 printf("enter a string:");
 gets(str);
 printf("string is %s\n",str);
 return 0;
  
}
