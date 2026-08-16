#include <string.h>
#include <stdio.h>
int main()
{
 char *p,str[100]="   main()";
 strcpy(str,str+strspn(str," \t"));
 puts(str);
 return 0;
  
}
