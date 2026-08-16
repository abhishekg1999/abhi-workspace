#include <string.h>
#include <stdio.h>
int main()
{
 char str[]="Vijaynagar";
 str=str+5;  //error array is a constant pointer
 printf("%s\n",str);
 return 0;
  
}
