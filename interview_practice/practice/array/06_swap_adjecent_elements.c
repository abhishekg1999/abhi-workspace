/*
intput: 
int a[] = {1, 2, 3, 4, 5, 6};

output : 
2 1 4 3 6 5

*/

#include <stdio.h>
void swap_adj(int a[], int ele){

	int i,j;
	int temp;

	for(i=0; i<ele-1; i=i+2){
		temp=a[i];
		a[i]=a[i+1];
		a[i+1]=temp;
	}

}

int main(){

	int a[] = {1, 2, 3, 4, 5, 6};
	int size = sizeof(a)/sizeof(a[0]);

	swap_adj(a, size);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);

	printf("\n");
	return 0;
}
