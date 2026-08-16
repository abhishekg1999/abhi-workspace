#include <stdio.h>
int main()
{
	int i,j,ele,a[6]={1,6,2,9,3,5};

	ele=sizeof(a)/sizeof(a[0]); //get number of elements

	printf("******before sort****\n");
	for(i=0;i<ele;i++)
		printf("%d\t",a[i]);

	for(i=0;i<ele-1;i++)
	{
		for(j=i+1;j<ele;j++)
			if(a[i]>a[j])
			a[i]=a[i]+a[j]-(a[j]=a[i]);
	}

	printf("\n******after sort****\n");
	for(i=0;i<ele;i++)
		printf("%d\t",a[i]);
	
	printf("\n");
}
