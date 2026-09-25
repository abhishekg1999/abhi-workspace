/*
Calculate sum and average

input:
int a[] = {10, 20, 30, 40, 50};

output:
sum =?
average =?

*/

#include <stdio.h>
void find_sum_avg(int a[], int ele){

	int sum = 0;
	double avg;

	for(int i=0; i<ele; i++)
		sum +=a[i];

	avg =(double)sum/ele;	
	printf("sum =%d avg =%f\n", sum, avg);
}

int main(){

	int a[] = {10, 20, 30, 40, 50};
	int size = sizeof(a)/sizeof(a[0]);

	find_sum_avg(a, size);
	printf("\n");
	return 0;
}
