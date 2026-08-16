#include <stdio.h>
int my_strlen(char*);
void my_strrev(char*);
int main()
{
	int i;
	char s[10];

	printf("enter the string\n");
	scanf("%s",s);
	printf("before string = %s\n",s);
	
	my_strrev(s);
	printf("after rev =%s\n",s);
}



