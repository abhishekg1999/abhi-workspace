/*
find Maximum difference

input:
int a[] = {7, 1, 5, 3, 6, 4};

like :
7-1 = 6 => maxdiff
5-1 = 4
3-1 = 2
6-1 = 5
4-1 = 3
So output:
7 - 1 = 6

*/

#include <stdio.h>
#include <limits.h>

void find_maxdiff(int a[], int ele){

        int maxdiff = INT_MIN;
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
                        if(diff > maxdiff) {
                                maxdiff = diff;
				num1 = large;
				num2 = small;
			}
                }
        }

        printf("%d - %d = %d\n", num1, num2, maxdiff);
}

#if 0
void find_maxdiff(int a[], int ele){

	int min_element = a[0];
	int max_element = a[0];

	for(int i=1; i<ele; i++){

		/* find min and max element in array */
		if( a[i] < min_element)
			min_element = a[i];
		if(a[i] > max_element)
			max_element = a[i];
	}

	/* calculate the maxdiff */
	int maxdiff = max_element - min_element;
	printf("%d - %d = %d\n", max_element, min_element, maxdiff);
}

#endif

int main(){

	int a[] = {7, 1, 5, 3, 6, 4};
	int size = sizeof(a)/sizeof(a[0]);

	find_maxdiff(a, size);
	return 0;
}
