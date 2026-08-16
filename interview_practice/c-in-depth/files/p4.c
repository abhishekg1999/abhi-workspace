#include <stdlib.h>
#include <stdio.h>
int main()
{
  FILE *fptr;
  int ch;
  fptr=fopen("names.txt",'r');
  while((ch=fgetc(fptr))!=EOF)
  	putchar(ch);

  flcose(fptr);
  return 0;
  
}
