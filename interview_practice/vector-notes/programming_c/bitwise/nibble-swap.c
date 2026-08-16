#include <stdio.h>
int main()
{
	unsigned char ch=0x23;
	int pos;
	for(pos=7;pos>=0;pos--)
		printf("%d",ch>>pos&1);


    	printf("\n");
	ch=ch>>4|ch<<4;

	for(pos=7;pos>=0;pos--)
		printf("%d",ch>>pos&1);
	
	printf("\n");
	return 0;
}
