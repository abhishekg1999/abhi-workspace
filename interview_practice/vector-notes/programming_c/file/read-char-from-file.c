#include <stdio.h>
int main()
{
	FILE *fp;
	char ch;
	fp=fopen("test","r");

	if(fp==0)
	{
		printf("file not present\n");
		return 0;
	}

	while((ch=fgetc(fp))!=-1)
		printf("%c",ch);

}
