#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	
	char a[] = "abcdefghijklmnopqrstuvwxyz";
	char b[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	printf("ascii of lower case:\n");
	for(int i=0; a[i]; i++)
		printf("%c = %d\n", a[i], a[i]);

	printf("ascii of upper case:\n");
	for(int i=0; b[i]; i++)
                printf("%c = %d\n", b[i], b[i]);

	return 0;
}
