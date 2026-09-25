/*
Find common elements or intersection

input:
int a[] = {1, 2, 3, 4, 5};
int b[] = {3, 4, 5, 6, 7};

output:
3 4 5

*/

#include <stdio.h>

void find_common(int a[], int ele1, int b[], int ele2){

	int i, j, k;
	for(i=0; i<ele1; i++){

		/* store already checked element */
		int checked=0;
		for(k=0; k<i; k++){

			if(a[i]==a[k])
				checked=1;
		}

		/* if already checked element, skip it */
		if(checked)
			continue;

		/* print matched elements just once */
		for(j=0; j<ele2; j++) {

			if(a[i] == b[j]){
				printf("%d ", a[i]);
				break;
			}
		}

	}
}

int main(){

	int a[] = {1, 2, 3, 4, 5};
	int b[] = {3, 3, 4, 5, 6, 7};

	int size1 = sizeof(a)/sizeof(a[0]);
	int size2 = sizeof(b)/sizeof(b[0]);
	
	find_common(a, size1, b, size2);

	printf("\n");
	return 0;
}
