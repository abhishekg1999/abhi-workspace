#include <stdio.h>
#define ROW 3
#define COL 3
int main()
{
	int a[ROW][COL]={{1,2,3},{4,5,6},{7,8,9}};
	int i,j,m,n,b[ROW][COL];

	printf("before rotate 90 degree\n");
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COL;j++)
			printf("%d ",a[i][j]);
		printf("\n");
	}

	//start--logic to rotate matrix 90 degree//
	for(j=2,m=0;j>=0;j--,m++)
		for(i=0,n=0;i<COL;i++,n++)
			b[m][n]=a[i][j];
	//end-- logic to rotate matrix 90 degree//

	printf("\nafter rotate 90 degree\n");
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COL;j++)
			printf("%d ",b[i][j]);
		printf("\n");
	}
	printf("\n");

}
