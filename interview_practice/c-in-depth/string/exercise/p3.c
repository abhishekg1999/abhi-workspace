//segmentation fault example //
#include <string.h>
#include <stdio.h>
int main()
{
 char *str1="good",*str2="morning";
 strcat(str1,str2);
 printf("%s\n",str1);
 return 0;
  
}
