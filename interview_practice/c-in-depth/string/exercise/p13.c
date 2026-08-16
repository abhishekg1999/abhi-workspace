#include <string.h>
#include <stdio.h>
int main()
{
 char str1[15]="Good";
 char str2[]="Everything";
 strcpy(str1+strlen(str1),str2);
 printf("%s\n",str1);
 return 0;
  
}
