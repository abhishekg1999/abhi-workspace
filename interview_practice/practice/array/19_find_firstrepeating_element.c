/*
Find the first repeating element

input:
int a[] = {10, 5, 3, 4, 3, 5, 6};

output:
5

*/

#include <stdio.h>

void find_firstrep(int a[], int ele){

	int i, j;
	for(i=0; i<ele; i++){

		for(j=i+1; j<ele; j++){
			if(a[i] == a[j]){

				printf("%d\n", a[i]);
				i=ele;   //put i to last, to termnate outer loop, no more iteration
				break;   //break
			}
		}
	}

}

int main(){

	int a[] = {10, 5, 3, 4, 3, 5, 6};
	int size = sizeof(a)/sizeof(a[0]);

	find_firstrep(a, size);
	return 0;
}
