/*
Find the first repeating element

input:
int a[] = {10, 5, 3, 4, 3, 5, 6};

output:
5

*/

#include <stdio.h>
#include <string.h>

void find_first_rep(int a[], int ele){

	int seen[ele];
	memset(seen ,0, ele);

	int i,j;
	for(i=0; i<ele; i++){

		if(seen[i] == 1)
			continue;

		int count =1;
		for(j=i+1; j<ele; j++){

			if(a[i] == a[j]){

				count++;
				seen[j]=1;
			};
		}

		if(count >1){
			printf("%d\n", a[i]);
			break;   //break loop after find first non repeated element
		}
	}
}

#if 0
void find_first_rep(int a[], int ele) {

	int count[256] = {0};
	int i,j;

	for(i=0; i<ele; i++){

		int val = a[i];
		count[val]++;
	}

	for(i=0; i<ele; i++){

		int val = a[i];
		if(count[val] >1){
			printf("%d\n", a[i]);
			break;   //break loop after find first non repeated element
		}
	}
}

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
#endif

int main(){

	int a[] = {10, 5, 3, 4, 3, 5, 6};
	int size = sizeof(a)/sizeof(a[0]);
	
	find_first_rep(a, size);
	return 0;
}
