#include <stdlib.h>
#include <stdio.h>
int main()
{
 FILE *fp;
 int ch;
 fp=fopen("myfile.txt","w");
 fprintf(fp,"if equal love there cannot be..");
 fputc(26,fp);
 fprintf(fp,"..let the more loving one be me\n");
 fclose(fp);
 fp=fopen("myfile.txt","r");

 while((ch=fgetc(fp))!=EOF)
 	putchar(ch);
	return 0;
  
}
