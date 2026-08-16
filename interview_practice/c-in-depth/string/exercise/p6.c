#include <string.h>
#include <stdio.h>
void func(char str[]);
int main()
{
 char str[]="Vijaynagar";
 func(str);
 return 0;
  
}

void func(char str[])  //here str is a pointer not an array
{
  str=str+5;
  printf("%s\n",str);
}
