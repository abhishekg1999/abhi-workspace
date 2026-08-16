#include <stdio.h>

int main()
{
	int i=0x12345678;
	char *cp=(char*)&i;

	if(*cp==0x78)
		printf("little endian\n");
	else
		printf("big endian\n");
}
