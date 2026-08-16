#include <stdio.h>
int main(int argc,char **argv)
{
	FILE *fs,*fd;
	char ch;
	if(argc!=3)
	{
		printf("usage : ./a.out file file\n");
		return ;
	}

	fs=fopen(argv[1],"r");
	if(fs==0)
	{
		printf("file not present\n");
		return ;
	}

	fd=fopen(argv[2],"w");
	while((ch=fgetc(fs))!=-1) /* upto end of the file */
		fputc(ch,fd);
}
