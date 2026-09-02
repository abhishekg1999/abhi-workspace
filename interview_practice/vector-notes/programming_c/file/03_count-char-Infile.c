#include <stdio.h>
int main(int argc,char **argv)
{
	if(argc!=3)
	{
		printf("usage:./a.out file char\n");
		return ;
	}

	FILE *fp=fopen(argv[1],"r");
	if(fp==0)
	{
		printf("file not present\n");
		return ;
	}

	int count=0;;
	char ch;

	while((ch=fgetc(fp))!=-1)  //read char by char from file
	{
		if(ch==argv[2][0])
			count++;
	}

	printf("char count=%d\n",count);
}
