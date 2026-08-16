#include <stdio.h>
int main()
{
	char s[10];
	int i;
	printf("enter the string\n");
	scanf("%s",s);
	
	printf("characters=");
	for(i=0;s[i];i++)
		printf("%c",s[i]);

	printf("\n");
}
