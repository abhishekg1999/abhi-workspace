#include <stdio.h>
int main()
{
	int a[5]={1,2,3,4,5};
	int i,temp,flag,roll;
	
	printf("how many times want to rotate \n");
	scanf("%d",&roll);i
    
	//Start-- array rotate logic //
	while(roll--)
	{
	
		flag=1;
		for(i=0;i<5;i++)
		{
			if(flag==1)
			{
				temp=a[0];
				flag=0;
			}
			a[i]=a[i+1];
		}
		a[4]=temp;
	}
	//End-- array rotate logic //

	for(i=0;i<5;i++)
		printf("%d ",a[i]);
	printf("\n");
}
