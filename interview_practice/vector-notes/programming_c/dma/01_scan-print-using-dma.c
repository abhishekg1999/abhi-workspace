#include <stdio.h>
#include <stdlib.h>
int main()
{
	char *p[3];  //array of char pointer
	int i;

	for(i=0;i<3;i++)
		p[i]=(char*)malloc(sizeof(char)*10);  //10 bytes allocated for string

	printf("enter the string\n");
	for(i=0;i<3;i++)
		scanf("%s",p[i]);

	for(i=0;i<3;i++)
		printf("%s ",p[i]);

	printf("\n");
}
