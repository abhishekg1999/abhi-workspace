/*
 * int a[] = {-2, 5, 0, -8, 10, 0, 7};
 * find : Positive = ? , Negative = ? , Zero = ?
*/

#include <stdio.h>
void count_pnz(int a[], int ele){

	int pos=0;
	int neg=0;
	int zero=0;

	for(int i=0; i<ele; i++){

		if(a[i]>0)
			pos++;
		else if(a[i]<0)
			neg++;
		else
			zero++;
	}

	printf("%d %d %d\n", pos, neg, zero);
}


int main(){

	int a[] = {-2, 5, 0, -8, 10, 0, 7};
	int size = sizeof(a)/sizeof(a[0]);

	count_pnz(a, size);
	return 0;
}
