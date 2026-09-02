/*
Find equilibrium index

An index where:
sum of elements on left = sum of elements on right

input:
int a[] = {1, 3, 5, 2, 2,7};

output:
3

Because:
1 + 3 + 5  = 2 + 7

*/

#include <stdio.h>

void find_queIndex(int a[], int ele){

	int i;
	int index;

	int leftsum=0; 
	int rightsum=0;
	int totalsum=0;

	for(i=0; i<ele; i++)
		totalsum += a[i];

	for(i=0; i<ele; i++) {
	
	   rightsum = totalsum - leftsum -a[i];

	   if(leftsum == rightsum){
		   index=i;
		   break;
	    }

	   leftsum += a[i];
	}

	printf("%d\n", index);
}

int main(){

	int a[] = {1, 3, 5, 2, 2, 7};
	int size = sizeof(a)/sizeof(a[0]);

	find_queIndex(a, size);
	return 0;
}
