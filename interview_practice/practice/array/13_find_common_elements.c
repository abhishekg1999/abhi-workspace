/*
Find common elements

input:
int a[] = {1, 2, 3, 4, 5};
int b[] = {3, 4, 5, 6, 7};

output:
3 4 5

*/

#include <stdio.h>

void find_common(int a[], int ele1, int b[], int ele2){

	int i;
	int seen[256] = {0};

	/* set 1 for each element in array a */
	for(i=0; i<ele1; i++){
		int val = a[i];
		seen[val] = 1;
	}

	/* find common */
	for(i=0; i<ele2; i++){

		int val = b[i];
		if(seen[val] == 1){
			
			printf("%d ", val);
			seen[val] = 0;   //once seen, make it 0
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
