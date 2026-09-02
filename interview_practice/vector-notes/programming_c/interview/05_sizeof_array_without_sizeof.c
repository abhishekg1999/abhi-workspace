#include <stdio.h>

int main()
{
	int a[5];
	int *p=a;
	int size = (char*)(p+5)-(char*)p;
	printf("size of array =%d\n",size);
	
	/* or 
	int size,ele=5;
	size=ele*4;
	printf("sizeo of array =%d\n",size);
	
	*/
}

