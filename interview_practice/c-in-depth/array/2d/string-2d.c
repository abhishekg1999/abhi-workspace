#include <stdio.h>
void main()
{
 char string[5][5];
 int ele,i;
 printf("enter the string\n");
 ele=sizeof(string)/sizeof(string[0]);
 for(i=0;i<ele;i++)
 scanf("%s",string[i]);

 for(i=0;i<ele;i++)
 printf("%s ",string[i]);

 printf("\n");
 }
