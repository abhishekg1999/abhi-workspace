#include <stdio.h>

//#define swap(a, b) a=a+b-(b=a)
#define swap(a, b) { int temp; \
		     temp =a; \
	             a=b;     \
	             b=temp;  \
	           }

int main(){

	int x=10, y=20;
	swap(x, y);

	printf("x=%d y=%d\n", x, y);
	return 0;
}
