/*
Separate even and odd numbers

input:
int a[] = {1, 2, 3, 4, 5, 6};

output:
2 4 6 1 3 5
*/

#include <stdio.h>
void seperate_even_odd(int a[], int ele){

	int i=0;  //pointing to odd number
	int j=ele-1;  //pointing to even number
	int temp; 

	while(i<j){

		/* pointing to odd (L->R), skip even */
		while( (a[i]%2==0) && i<j)
			i++;

		/* pointing to even (L<-R), skip odd */
		while( (a[j]%2 !=0) && i<j)
                        j--;

		temp=a[i];
		a[i]=a[j];
		a[j]=temp;
	}
}

int main(){

	int a[] = {1, 2, 3, 4, 5, 6};
	int size = sizeof(a)/sizeof(a[0]);

	seperate_even_odd(a, size);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);

	printf("\n");
	return 0;
}
