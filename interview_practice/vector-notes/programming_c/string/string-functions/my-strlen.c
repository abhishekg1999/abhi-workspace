#include <stdio.h>
int my_strlen(char*);
int main()
{
	char s[20];
	int l;

	printf("enter the string\n");
	scanf("%s",s);
	printf("%d\n",my_strlen(s));

}

int my_strlen(char *p)
{

	int i;
	for(i=0;p[i];i++);

	return i;
}
