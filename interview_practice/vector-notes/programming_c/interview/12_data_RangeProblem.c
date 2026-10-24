#include <stdio.h>

int main(){

	unsigned int i;
	for(i=9; i>=0; i--)
		printf("%d\n", i);

	return 0;
}

/* it will go infinite because of unsigend int so need to replace 
 * unsigend int i ==> int i;
 * unsigned int can not store -ve number
*/
