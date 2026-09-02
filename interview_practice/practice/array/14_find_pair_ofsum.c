/*
Find pair with given sum

input:
int a[] = {2, 7, 11, 15};
int target = 9;

output:
2 + 7 = 9

*/

#include <stdio.h>
void find_pair(int a[], int ele){

	int i,j;
	for(i=0; i<ele; i++){

		for(j=0; j<ele; j++){
			if( (a[i]+a[j] ==9) && a[i]!=a[j]){   //check sum and number should not be same
				printf("%d %d", a[i], a[j]);
				i=ele;   //increment i to last, to terminate outer loop
				break;  
			}
		}
	}
}

int main(){

	int a[] = {2, 7, 11, 15, 2, 7};
	int size = sizeof(a)/sizeof(a[0]);

	find_pair(a, size);
	
	printf("\n");
	return 0;
}
