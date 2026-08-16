#include <stdio.h>
int main(int argc, char *argv[])
{
//	printf("argc=%d\targv[0]=%lu\targv[1]=%lu\targv[2]=%lu\n",argc,argv[0],argv[1],argv[2]);
//	printf("argv[0]=%s\targv[1]=%s\targv[2]=%s\n",argv[0],argv[1],argv[2]);
	printf("argc=%d\n",argc);

	for(int i=0;i<argc;i++)
	{
		printf("argv[%d]=%s\n",i,argv[i]);
	}
}
