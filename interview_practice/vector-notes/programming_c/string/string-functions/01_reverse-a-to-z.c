#include <stdio.h>
void my_strrev_1(char*,char*); //reverse between location
void my_strrev(char*);				//reverese string
char* my_strchr(char*,char);	//found char location

int main()
{
	char s[30],*p,*q;
	printf("enter the string\n");
	scanf("%[^\n]",s);
	printf("before =%s\n",s);
	p=s;

	while(q=my_strchr(p,' '))
	{
		my_strrev_1(p,q-1);
		p=q+1;
	}

	my_strrev(p);
	printf("after =%s\n",s);
}
