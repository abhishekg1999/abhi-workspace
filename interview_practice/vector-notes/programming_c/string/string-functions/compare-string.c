#include <stdio.h>
int my_strcmp(char*,char*);

int main()
{
	char s[10],d[10];
	printf("enter the string\n");
	scanf("%s%s",s,d);

	if(my_strcmp(s,d)==0)
		printf("both strings equal\n");
	else if(my_strcmp(s,d)==1)
		printf("first string is greater \n");
	else
		printf("2nd string is greater\n");
}
