#include <stdio.h>

int main(){

	int i=10, j;
	int *p =&i;

	//j = ++*p;  // or j = ++(*p)
	//j = *++p;   
	//j = *p++;
	j = (*p)++;
	printf("i=%d j=%d *p=%d\n", i, j ,*p);
	return 0;
}
