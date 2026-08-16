#include <stdlib.h>
#include <stdio.h>
int main()
{
 char str1[]="hockey";
 char str2[]="cricket";
 char const *p=str1;
 *p='j';
 p=str2;
 return 0;
  
}
