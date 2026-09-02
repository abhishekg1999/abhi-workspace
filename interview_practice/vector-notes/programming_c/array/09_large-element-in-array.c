#include <stdio.h>
int main()
{
	int a[5];
	int l,i,index,ele;
	ele=sizeof(a)/sizeof(a[0]);
    
	printf("enter the elements\n");
	for(i=0;i<ele;i++)
		scanf("%d",&a[i]);

	l=a[0];
	index=0;
	for(i=1;i<ele;i++)
		if(a[i]>l)
		{
			l=a[i];
			index=i;
		}

	printf("large=%d\n",l);
}
