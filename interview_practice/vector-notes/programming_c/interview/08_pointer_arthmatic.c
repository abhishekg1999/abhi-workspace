#include <stdio.h>

int main(){

	int a[5] = {10,20,30,40,50};

	int *ptr1 = &a[1];
       	int *ptr2 = &a[4];

	printf("%ld\n", ptr2-ptr1);
	printf("%ld\n", (char*)ptr2 - (char*)ptr1);
	
	return 0;
}	

