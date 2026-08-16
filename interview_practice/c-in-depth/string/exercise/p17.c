#include <string.h>
#include <stdio.h>
int main()
{
 char str1[]="Paul",str2[]="Devanshi";
 printf("%d\n",strlen(str1)-strlen(str2));
 if(strlen(str1)-strlen(str2)>=0) //after solve => if( 4>=0)
 	puts(str1);

 else
 	puts(str2);

 return 0;
  
}
