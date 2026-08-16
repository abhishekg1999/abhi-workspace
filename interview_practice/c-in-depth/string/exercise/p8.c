#include <string.h>
#include <stdio.h>
int main()
{
 char *p="Devanshi";
 char arr[]="Devanshi";
 *(p+2)='b'; //error string stored read only section
 arr[2]='b'; //error array is const pointer
 puts(p);
 puts(arr);
 return 0;
  
}
