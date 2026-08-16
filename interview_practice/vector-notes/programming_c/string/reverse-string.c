#include <stdio.h>
int main()
{
	int i,j;
	char s[10];

	printf("enter the string\n");
	scanf("%s",s);

	printf("before=%s\n",s);
	for(i=0;s[i];i++);

	for(j=0,i=i-1;j<i;j++,i--)
		s[j]=s[j]+s[i]-(s[i]=s[j]);

	printf("after=%s\n",s);
}

