#include <stdio.h>
int main()
{
	int i=258;
	char *cp=(char*)&i;

	for(int j=0;j<4;j++)
	{
		printf("%d ",*cp);
		cp++;
	}
	printf("\n");
}
