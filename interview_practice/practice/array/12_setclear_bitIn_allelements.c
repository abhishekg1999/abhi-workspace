/* Set/Clear array elements based on condition */

#include <stdio.h>

#define SETBIT(num, pos) num|=(1<<pos)

void set_clr_elements(int a[], int ele, int pos) {

	for(int i=0; i<ele; i++){
		SETBIT(a[i], pos);
	}

}

int main(){

	int a[] = {4,8,12};
	int size = sizeof(a)/sizeof(a[0]);

	set_clr_elements(a, size, 1);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);

	printf("\n");
	return 0;
}
