/*
Product of array except self

input:
int a[] = {1, 2, 3, 4};

output:
24 12 8 6

Meaning:
24 = 2*3*4
12 = 1*3*4
8  = 1*2*4
6  = 1*2*3

*/

#include <stdio.h>

void find_product(int a[], int ele){

	int i,j;
	int product;

	for(i=0; i<ele; i++){
	
		product=1;
		for(j=0; j<ele; j++){

			if(a[i] == a[j])
				continue;

			product *= a[j];
		}

		printf("%d ", product);
	}

}

int main(){

	int a[] = {1, 2, 3, 4};
	int size = sizeof(a)/sizeof(a[0]);

	find_product(a, size);
	printf("\n");
	
	return 0;
}
