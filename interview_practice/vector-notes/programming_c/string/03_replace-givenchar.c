#include <stdio.h>
int main()
{
	char s[10],ch,ch1;
	int i;

	printf("enter the string\n");
	scanf("%s",s);

	printf("enter the char1 and char2\n");
	scanf(" %c %c",&ch,&ch1);

	for(i=0;s[i];i++)
	{
		if(s[i]==ch)
			s[i]=ch1;

	}
	
	printf("%s\n",s);
}
