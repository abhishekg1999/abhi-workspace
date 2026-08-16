#include <stdlib.h>
#include <stdio.h>
int main()
{
 char name[50];
 int empid;
 fprintf(stdout,"Enter your name: ");
 fgets(name,50,stdin);
 fprintf(stdout,"Enter your empid: ");
 fscanf(stdin,"%d",&empid);
 fprintf(stdout,"your empid is: %d",empid);
 fputc('\n',stdout);
 fprintf(stdout,"your name is: ");
 fputs(name,stdout);
 return 0;
}
