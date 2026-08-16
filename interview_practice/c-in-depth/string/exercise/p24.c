#include <string.h>
#include <stdio.h>
void func(char *p);
int main()
{
 char *str="doubtful";
 func(str);
 return 0;
  
}

void func(char *p)
{
  if(*p!='f')
  {
    printf("%c",*p);
	func(++p);
  }
}
