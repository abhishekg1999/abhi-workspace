#include <stdio.h>
int main()
{
	float f=23.5;
	char *p;
	int pos;
	p=(char*)&f;

	for(p=p+3;p>=&f;p--)
	{
		for(pos=7;pos>=0;pos--)
			printf("%d",*p>>pos&1);

		printf(" ");
	}

	/*for(pos=31;pos>=0;pos--)
		printf("%d",*p>>pos&1);*/
	printf("\n");
}
	

