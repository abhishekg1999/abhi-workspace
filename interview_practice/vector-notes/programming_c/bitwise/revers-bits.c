#include <stdio.h>
int main()
{
	int num,pos,i,j,m,n;
	printf("enter the number\n");
	scanf("%d",&num);

	printf("********print before ******\n");
	for(pos=31;pos>=0;pos--)
		printf("%d",num>>pos&1);
	
	//logic of bit reverse//
	for(i=0,j=31;i<j;i++,j--)
	{
		//m=num>>i&1;
		//n=num>>j&1;

		if((num>>i&1)^(num>>j&1))
		{
			num=num^1<<i;
			num=num^1<<j;
		}
	}

	printf("\n*******print after ******\n");

	for(pos=31;pos>=0;pos--)
		printf("%d",num>>pos&1);

	printf("\n");
}


