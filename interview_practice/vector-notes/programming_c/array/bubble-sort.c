#include <stdio.h>
int main()
{
	int i,j,a[6]={1,4,3,9,6,10};
    	int ele;
	ele=sizeof(a)/sizeof(a[0]);
    	printf("*****before sort******\n");
	for(i=0;i<ele;i++)
		printf("%d\t",a[i]);

	for(i=0;i<ele-1;i++)
	{
		for(j=0;j<ele-1-i;j++)
			if(a[j]>a[j+1])
				a[j]=a[j]+a[j+1]-(a[j+1]=a[j]);
	}

	printf("\n*****after sort******\n");
	for(i=0;i<ele;i++)
		printf("%d\t",a[i]);

	printf("\n");
}
		
	

