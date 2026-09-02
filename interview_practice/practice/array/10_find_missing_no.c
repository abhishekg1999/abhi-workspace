/*
find missing element in array 

input:
int a[] = 1, 2, 3, 5, 6

output:
4

*/

#include <stdio.h>

int find_missing(int a[], int ele){

	int i;
	int temp=-1;
	int j=1;

	for(i=0; i<ele; i++, j++){

		if(a[i] != j){
			temp=j;
			break;
		}
	}

	return temp;
}

int main(){

	int a[] = {1, 2, 3, 5, 6};
       	int size = sizeof(a)/sizeof(a[0]);

	int ret = find_missing(a, size);
	printf("ret =%d\n", ret);

	return 0;
}	
