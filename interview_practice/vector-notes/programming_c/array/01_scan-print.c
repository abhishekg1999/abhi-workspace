#include <stdio.h>
int main()
{

	int i,ele,a[3];
	ele=sizeof(a)/sizeof(a[0]);

	printf("enter the array \n");
	for(i=0;i<ele;i++)
		scanf("%d",&a[i]);

	printf("*******array******\n");
	for(i=0;i<ele;i++)
		printf("%d ",a[i]);

	printf("\n*****reverse******\n");
		for(i=ele-1;i>=0;i--)
			printf("%d ",a[i]);
	
	printf("\n");
}
