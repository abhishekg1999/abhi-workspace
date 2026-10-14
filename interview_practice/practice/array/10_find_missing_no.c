/*
find missing element in array 

input:
int a[] = {1,2,4,6,3,7,8};

output:
5

*/

#include <stdio.h>

#if 0
int find_miss(int a[], int ele){

	/* the range of numbers is from 1 to n, so n = array elements + missing element */
	int n = ele+1;  

	/* calculate the expected sum of all numbers from 1 to n using the standard mathamatical formula */
	int expected_sum = n*(n+1)/2;
	int actual_sum = 0;
	for(int i=0; i<ele; i++)
		actual_sum +=a[i];

	/* get missing element */
	int ret = expected_sum - actual_sum;
	return ret;
}
#endif

int find_miss(int a[], int ele){

	int n = ele+1;
	int xor_expected = 0;
	int xor_actual = 0;
	
	/* xor all numbers from 1 to n */
	for(int i=1; i<=n; i++)
		xor_expected ^=i;

	/* xor all element present in the way */
	for(int i=0; i<ele; i++)
		xor_actual ^=a[i];

	int ret = xor_expected^xor_actual;
	return ret;
}

int main(){

	int a[] = {1,2,4,6,3,7,8};
       	int size = sizeof(a)/sizeof(a[0]);

	int ret = find_miss(a, size);
	printf("%d\n", ret);
	return 0;
}	
