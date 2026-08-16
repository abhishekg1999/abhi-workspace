#include <stdio.h>
int main()
{
	int num,pos;
	printf("ente the number\n");
	scanf("%d",&num);

	for(pos=31;pos>=0;pos--)
	{
		printf("%d",num>>pos&1);
		if(pos%8==0)
			printf(" ");
	}
	
	printf("\n");
}
	
