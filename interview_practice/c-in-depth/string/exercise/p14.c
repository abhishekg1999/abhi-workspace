#include <string.h>
#include <stdio.h>
int main()
{
 char str1[]="parul";
 char str2[10];
 strcpy(str2,str1);
 if(str1==str2)
 	printf("same\n");
 else
 	printf("different\n");

 return 0;
  
}
