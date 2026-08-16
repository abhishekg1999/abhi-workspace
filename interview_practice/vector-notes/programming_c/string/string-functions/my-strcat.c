#include <stdio.h>
void my_strcat(char*,char*);

int main()
{
	char f[20],s[10];

	printf("enter the strings f:s\n");
	scanf("%s%s",f,s);

	printf("before f=%s\ts=%s\n",f,s);
	my_strcat(f,s);

	printf("after f=%s\ts=%s\n",f,s);
}
