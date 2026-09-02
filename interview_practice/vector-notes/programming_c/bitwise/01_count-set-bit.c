#include <stdio.h>
int main()
{
	int num,pos,count=0;
	printf("enter the number\n");
	scanf("%d",&num);

	for(pos=31;pos>=0;pos--)
		if(num>>pos&1)
		count++;
	printf("count=%d\n",count);

	return 0;
}
