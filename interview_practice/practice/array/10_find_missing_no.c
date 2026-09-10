/*
find missing element in array 

input:
int a[] = 1, 2, 3, 5, 6

output:
4

*/

#include <stdio.h>

void find_missing(int a[], int ele){

	int i, j;
	
	for(i=0, j=1; i<ele; i++, j++){

		if(a[i] != j){
			printf("%d\n", j);
			break;
		}
	}
}

int main(){

	int a[] = {1, 2, 3, 5, 6};
       	int size = sizeof(a)/sizeof(a[0]);

	find_missing(a, size);
	return 0;
}	
