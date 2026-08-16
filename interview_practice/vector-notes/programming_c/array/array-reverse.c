#include <stdio.h>

int main()
{
	int ele,i,j,a[10]={1,2,3,4,5,6,7,8,9,10};
	ele=sizeof(a)/sizeof(a[0]);

	printf("******before \n");
	for(i=0;i<ele;i++)
		printf("%d\t",a[i]);

	for(i=0,j=ele-1;i<j;i++,j--)
		a[i]=a[i]+a[j]-(a[j]=a[i]);

	printf("\n*******after reverse\n");
	for(i=0;i<ele;i++)
		printf("%d\t",a[i]);
	
	printf("\n");
}
