/*
Remove duplicates from a sorted array

input:
int a[] = {1, 1, 2, 2, 2, 3, 4, 4, 5};

output:
Modify the same array so that the unique elements are at the beginning:
1 2 3 4 5

*/

#include <stdio.h>


void remove_dup(int a[], int *ele){

	int i, j, k;
	for(i=0; i<(*ele); i++){

		for(j=i+1; j<(*ele); j++){

			if(a[i] == a[j]){
				
				for(k=j; k<(*ele); k++)
					a[k]=a[k+1];
				
				j--;
				(*ele)--;
			}
		}
	}
}

#if 0
void remove_dup(int a[], int *ele){

	int i, j;
	int seen[256]= {0};

	for(i=0, j=0; i<(*ele); i++){

		int val = a[i];
		if(seen[val] == 0){

			a[j++] = a[i];  //copy value which is coming only once
			seen[val]=1;   //set 1, if element seen
		}
	}

	*ele = j;
}
#endif

int main(){

	int a[] = {1, 1, 2, 2, 2, 3, 4, 4, 5};
	int size = sizeof(a)/sizeof(a[0]);

	remove_dup(a, &size);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);

	printf("\n");
	return 0;
}
