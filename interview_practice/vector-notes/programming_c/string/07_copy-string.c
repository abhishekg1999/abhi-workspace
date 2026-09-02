#include <stdio.h>
int main()
{
	char s[10],d[10];
	int i;
	printf("enter source  string\n");
	scanf("%s",s);

	for(i=0;s[i];i++)
		d[i]=s[i];

	d[i]=s[i];

	printf("source =%s desti =%s\n",s,d);
}


