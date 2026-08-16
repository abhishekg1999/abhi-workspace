#include <stdio.h>
int main()
{
  int i,total=0;
  for(i=1;i<=10;i++)
  {
  		switch(i)
		{
			case 1: 
				printf("case 1 ");
			case 4:
				printf("case 4 ");
			case 5:
				printf("case 5 ");
			case 7:
				printf("case 7 ");
				total+=i;
				break;
			default:
				continue;
		}
		printf("\n");
	//	printf("%d ",i);
	}
	printf("total=%d\n",total);
	return 0;
}
