#include <stdio.h>
int main()
{
	int i,j,a[10]={11,12,22,13,14,15,16,44,55,66};
	int ele,count=0;
	ele=sizeof(a)/sizeof(a[0]);

	for(i=0;i<10;i++)
	{
		for(j=2;j<a[i];j++)
		{
			if(a[i]%j==0)
			break;
		}

		if(j==a[i])
			count++;
	}
	printf("count=%d\n",count);
}
