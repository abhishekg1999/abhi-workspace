#include <stdio.h>
int main()
{
	FILE *fp;   //file pointer 

	fp=fopen("test_file","r");
	if(fp==0)
		printf("not present\n");
	else
		printf("file present\n");
}
