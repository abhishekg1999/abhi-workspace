#include <stdio.h>
int main()
{
	int i,j;
	char s[10];

	printf("enter the string\n");
	scanf("%s",s);

	printf("before=%s\n",s);
	for(j=0;s[j];j++);

	for(i=0,j=j-1;i<j;i++,j--)
		s[i]=s[i]+s[j]-(s[j]=s[i]);

	printf("after=%s\n",s);
}

