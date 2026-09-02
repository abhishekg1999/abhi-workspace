/*
Remove an element in-place

input:
int a[] = {10, 20, 30, 20, 40, 50};
int value = 20;

Remove all occurrences of 20 in-place.

output:
10 30 40 50

*/

#include <stdio.h>

void remove_element(int a[], int *ele){

	for(int i=0; i<(*ele); i++){

		if(a[i]==20){

			for(int j=i; j<(*ele); j++)
				a[j]=a[j+1];
			
			i--;        //for consicutive number
			(*ele)--;  //decrease size
		}
	}

}

int main(){

	int a[] = {10, 20, 20, 30, 20, 40, 50};
	int size = sizeof(a)/sizeof(a[0]);

	remove_element(a, &size);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);
	
	printf("\n");
	return 0;
}
