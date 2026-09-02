/*
Find two numbers with minimum difference
int a[] = {10, 3, 5, 8, 20, 7};

Find the pair having the smallest absolute difference.

Expected:

7, 8

*/

#include <stdio.h>

void bubble_sort(int a[], int ele){

	int i,j;
	int temp;

	for(i=0; i<ele-1; i++){

		for(j=0; j<ele-1-i; j++){

			if(a[j]>a[j+1]){
				temp=a[j];
				a[j]=a[j+1];
				a[j+1]=temp;
			}
		}
	}
}

void find_mindiff(int a[], int ele){

	int i;
	int num1, num2;
	int min_diff=a[1]-a[0];
	
	for(i=1; i<ele-1; i++){

		int diff = a[i+1]-a[i];
		if(diff < min_diff){
			min_diff = diff;
			num1=a[i+1];
			num2=a[i];
		}
	}

	printf("%d - %d = %d\n", num1, num2, min_diff);
}

int main(){

	int a[] = {10, 3, 5, 8, 20, 7};
	int size = sizeof(a)/sizeof(a[0]);

	bubble_sort(a, size);

	find_mindiff(a, size);

	return 0;
}
