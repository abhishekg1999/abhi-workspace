#include <stdio.h>
int main()
{
	int num,i,count;

	for(num=50,count=0;count<15;num++)
	{
		for(i=2;i<num;i++)
		 	if(num%i==0)
			break;

		if(i==num)
		{
			count++;
			printf("%d\t",num);
		}
	}
	printf("count=%d \n",count);
}
