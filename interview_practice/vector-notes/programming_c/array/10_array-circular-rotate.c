#include <stdio.h>
int main()
{
	int a[5]={1,2,3,4,5};
	int ele=sizeof(a)/sizeof(a[0]);
	int i,temp,roll;
	
	printf("No of rotations \n");
	scanf("%d",&roll);
    
	//Start-- array rotate logic //
	while(roll--)
	{
		temp=a[0];	
		for(i=0;i<ele;i++)
			a[i]=a[i+1];
		
		a[ele-1]=temp;  //feed front value at the end
	}
	//End-- array rotate logic //

	for(i=0;i<5;i++)
		printf("%d ",a[i]);
	printf("\n");
}
