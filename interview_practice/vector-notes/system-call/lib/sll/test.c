#include <stdio.h>
#include "mylib.h"

int main()
{
	int r;
	print("hello");

	r=sum(10,20);
	printf("sum=%d\n",r);
	r=sub(20,10);
	printf("sub=%d\n",r);
}


