#include <stdlib.h>
#include <stdio.h>
int main()
{
  FILE *fptr;
  unsigned char ch;
  fptr=fopen("myfile.txt","r");
  while((ch=fgetc(fptr))!=EOF)
  	putchar(ch);

	fclose(fptr);
	return 0;
  
}
