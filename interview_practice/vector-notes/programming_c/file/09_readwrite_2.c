#include <stdio.h>
int main()
{
	int a[5]={10,20,30,40,50};
	int b[5],i;

	FILE *fp=fopen("data","w+");

	//print data into file
	for(i=0;i<5;i++)
		fprintf(fp,"%d ",a[i]);

	rewind(fp);
	//scan data from file
	for(i=0;i<5;i++)
		fscanf(fp,"%d ",&b[i]);

	for(i=0;i<5;i++)
		printf("%d ",b[i]);

	printf("\n");
}

