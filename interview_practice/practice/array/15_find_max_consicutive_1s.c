/*
Find maximum consecutive 1s

input:
int a[] = {1, 1, 0, 1, 1, 1, 0, 1};

output:
3

*/

#include <stdio.h>
void find_max_1s(int a[], int ele){

	int temp=0;
	int count=0;

	for(int i=0; i<ele; i++){

		if(a[i] == 1)
			temp++;
		else if(temp>count) {
				count=temp;
				temp=0;
		}
		
	}
	
	printf("%d\n", count);
}

int main(){

	int a[] = {1, 1, 0, 1, 1, 1, 1, 0, 1};
	int size = sizeof(a)/sizeof(a[0]);

	find_max_1s(a, size);
	return 0;
}
