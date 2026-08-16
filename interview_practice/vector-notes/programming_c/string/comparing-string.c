#include <stdio.h>
int main()
{
	int i,j;
	char a[10],b[10];

	printf("enter the string a:b\n");
	scanf("%s%s",a,b);

	for(i=0;a[i];i++)
	{
		if(a[i]!=b[i])
		break;
	}
	if(a[i]==b[i])
		printf("equal\n");

	else
		printf("not equal\n");
}
