#include <stdio.h>
void my_strcpy(char *,char *);
int main()
{
	int i,j;
	char s[10],ch;

	printf("enter the string \n");
	scanf("%s",s);
	printf("enter char\n");
	scanf(" %c",&ch);
	
	for(i=0;s[i];i++)
	{
		/*if(s[i]==ch)
		{
			my_strcpy(s+i+1,s+i);
			i--;
		}*/
		if(s[i]==ch)
		{
			for(j=i;s[j];j++)
				s[j]=s[j+1];

			i--; /* for same elements in series */
		}
	}
	
	printf("string after delete= %s\n",s);
}
