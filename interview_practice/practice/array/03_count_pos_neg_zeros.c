/*
 * int a[] = {-2, 5, 0, -8, 10, 0, 7};
 * find : Positive = ? , Negative = ? , Zero = ?
*/

#include <stdio.h>
void count_pnz(int a[], int ele, int *positive, int *negative, int *zero){

	int i;
	for(i=0; i<ele; i++){

		if(a[i]>0)
			(*positive)++;
		else if(a[i]<0)
			(*negative)++;
		else
			(*zero)++;
	}
}


int main(){

	int a[] = {-2, 5, 0, -8, 10, 0, 7};
	int size = sizeof(a)/sizeof(a[0]);

	int pos=0, neg=0, zero=0;
	count_pnz(a, size, &pos, &neg, &zero);
	printf("positive=%d negative=%d zero=%d\n", pos, neg, zero);
	return 0;
}
