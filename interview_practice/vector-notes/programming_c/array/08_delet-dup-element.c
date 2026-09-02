#include <stdio.h>
int main()
{
	int i,j,k,ele;
	int a[8]={1,2,2,3,4,4,5,6};
	ele=sizeof(a)/sizeof(a[0]);
	
	
	/* logic 1
	for(i=0;i<ele;i++)
	{
		for(j=i+1;j<ele;j++)
		{
			if(a[i]==a[j])
			{
				for(k=j;k<ele;k++)
					a[k]=a[k+1];

			j--;   //if more continue dup elements coming
			ele--;	//if one ele delete then decrease no of ele
			}
		}
	}
	*/

	/* logic 2 , first sort the array than use this logic */
        j=0;   //index 0
        for(i=1; i<ele; i++) {
           if(a[i]!=a[j])
             a[++j]=a[i];
        }
        ele = j+1; //update ele after remove duplicates

	//print array
	for(i=0;i<ele;i++)
		printf("%d ",a[i]);

	printf("\n");
}
