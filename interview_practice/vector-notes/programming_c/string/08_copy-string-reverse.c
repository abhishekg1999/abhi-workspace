#include <stdio.h>
int main()
{
	int i,j;
	char s[10],d[10];

	printf("enter the string\n");
	scanf("%s",s);

	for(i=0;s[i];i++);

	for(i=i-1,j=0;i>=0;i--,j++)
		d[j]=s[i];

		d[j]='\0';

	printf("s= %s\td= %s\n",s,d);
}
