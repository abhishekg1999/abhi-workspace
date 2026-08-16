#include <stdio.h>
#include <string.h>
int main(int argc,char **argv)
{
	char s[30];  //define string
	int i,count;

	FILE *fp=fopen(argv[1],"r");
	if(fp==0)
	{
		printf("file not present\n");
		return ;
	}

	count=0;
	while(fscanf(fp,"%s",s)!=-1)
	{
		if(strcmp(s,argv[2])==0)
			count++;

	}

	printf("count=%d\n",count);
}

