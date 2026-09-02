#include <stdio.h>
int main()
{
	char f[10],s[10];


	int i,j;
	printf("enter the string\n");
	scanf("%s%s",f,s);

	for(i=0;f[i];i++);

	for(j=0;s[j];j++,i++)
		f[i]=s[j];

	f[i]=s[j];
	printf("f=%s\ts=%s\n",f,s);

}

