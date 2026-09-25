/*
find Maximum difference

input:
int a[] = {7, 1, 5, 3, 6, 4};

j > i (The element you subtract from must come after the smaller element).
like :
5-1 = 4
3-1 = 2
6-1 = 5 => maxdiff
4-1 = 3

So output:
6 - 1 = 5

*/

#include <stdio.h>
#include <limits.h>

void find_diff(int a[], int ele){

	int min_element = a[0];
	int maxdiff = INT_MIN;

	for(int i=1; i<ele; i++){

		if( a[i]-min_element > maxdiff)
			maxdiff=a[i]-min_element;

		if(a[i]<min_element)
			min_element=a[i];
	}

	printf("%d\n", maxdiff);
}

int main(){

	int a[] = {7, 1, 5, 3, 6, 4};
	int size = sizeof(a)/sizeof(a[0]);

	find_diff(a, size);

	printf("\n");
	return 0;
}
