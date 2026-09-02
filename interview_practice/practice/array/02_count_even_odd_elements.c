/*
Count even and odd numbers
int a[] = {10, 15, 22, 7, 8, 13, 20};

Expected:

Even = 4
Odd  = 3
*/

#include <stdio.h>

void count_even_odd(int a[], int ele){

	int i;
	int even=0, odd=0;

	for(i=0; i<ele; i++){

		if(a[i]%2==0)
			even++;
		else
			odd++;
	}
	
	printf("even =%d odd =%d\n", even, odd);
}

int main(){

	int a[] = {10, 15, 22, 7, 8, 13, 20};
	int size = sizeof(a)/sizeof(a[0]);

	count_even_odd(a, size);
	return 0;
}
