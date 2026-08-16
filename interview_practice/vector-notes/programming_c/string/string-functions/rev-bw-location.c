#include <stdio.h>
void my_strrev_1(char *,char *);

int main()
{
	char s[10];
	printf("enter the string\n");
	scanf("%s",s);

	printf("before =%s\n",s);
	my_strrev_1(s+1,s+5);

	printf("after =%s\n",s);
}
