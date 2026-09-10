/*
find the min difference between two elements 

input:
7, 1, 5, 3, 6, 4

output:
1
*/

#include <stdio.h>
#include <limits.h>

void find_mindiff(int a[], int ele){

	int mindiff = INT_MAX;
	int num1, num2;

	for(int i=0; i<ele; i++){
	
		for(int j=i+1; j<ele; j++) {
		
			int large, small;
			if(a[i] > a[j]) {
				large = a[i];
				small = a[j];
			}
			else {
				large = a[j];
                                small = a[i];
			}

			int diff = large-small;
			if(diff < mindiff) {
				mindiff = diff;
				num1 = large;
				num2 = small;
			}
		}
	}

	printf("%d - %d = %d\n", num1, num2, mindiff);
}

int main(){

	int a[] = {7, 1, 5, 3, 6, 4};
        int size = sizeof(a)/sizeof(a[0]);

        find_mindiff(a, size);
        return 0;
}
