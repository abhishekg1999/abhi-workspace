#include <stdio.h>
#include <string.h>
int main(int argc,char **argv)
{
	int i;
	if(argc<2)
	{
		printf("use ./a.out string\n");
		return ;
	}

	i=strlen(argv[1]);
	printf("string=%s\tl=%d\n",argv[1],i);
}
