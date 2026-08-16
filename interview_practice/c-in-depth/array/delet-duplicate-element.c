#include <stdio.h>
int main()
{
	int i,j,k,ele;
	int a[8]={1,2,2,3,4,4,5,6};
	ele=sizeof(a)/sizeof(a[0]);
	
	
	//start -- logic of delete duplicate ele
	for(i=0;i<ele;i++)
	{
		for(j=i+1;j<ele;j++)
		{
			if(a[i]==a[j])
			{
				for(k=j;k<ele;k++)
					a[k]=a[k+1];

			j--;   //if more dup ele
			ele--;	//if one ele delete then decrease no of ele
			}
		}
	}
	//End -- logic to delete duplicate ele

	for(i=0;i<ele;i++)
		printf("%d ",a[i]);

	printf("\n");
}
