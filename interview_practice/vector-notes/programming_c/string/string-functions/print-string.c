#include <stdio.h>
void print_string(char *p);
int main()
{
	char s[20];
	printf("enter the string\n");
	scanf("%s",s);
	print_string(s);
}

void print_string(char *p)
{

	while(*p)
		printf("%c",*p++);
	
	printf("\n");
}

