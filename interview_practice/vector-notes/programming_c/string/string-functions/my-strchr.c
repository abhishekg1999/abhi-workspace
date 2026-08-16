#include <stdio.h>
char* my_strchr(char*,char);
int main()
{
	char s[10],*p,ch;
	printf("enter the string\n");
	scanf("%s",s);

	printf("enter the char\n");
	scanf(" %c",&ch);

	p=my_strchr(s,ch);
	if(p==0)
		printf("char not present\n");

	else
		printf("char adds =%p\n",my_strchr);
}
