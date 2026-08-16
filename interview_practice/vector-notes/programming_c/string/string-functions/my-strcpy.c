#include <stdio.h>
void my_strcpy(char *, const char *);

int main()
{
	char s[10],d[10];
	printf("enter the strings\n");

	scanf("%s",s);
	my_strcpy(d,s);
	printf("s=%s\td=%s\n",s,d);
}

