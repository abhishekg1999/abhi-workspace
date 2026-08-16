#include <stdio.h>
#include <string.h>
int main()
{
	char s[10];
	printf("enter the string \n");
	fgets(s,sizeof(s),stdin);
	printf("%s",s);
}
