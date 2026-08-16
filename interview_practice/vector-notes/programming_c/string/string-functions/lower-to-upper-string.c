#include <stdio.h>
void my_strupr(char*);

int main()
{
	char s[10];
	printf("enter the string\n");
	scanf("%s",s);

	printf("before =%s\n",s);
	my_strupr(s);

	printf("after =%s\n",s);
}
