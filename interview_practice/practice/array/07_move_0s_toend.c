/*
Move all zeros to the end

input:
int a[] = {0, 1, 0, 3, 12};

output:
1 3 12 0 0

*/

#include <stdio.h>

void move_zero_toend(int a[], int ele){

	int i;
	int index=ele;  //index of first 0 in array

	for(i=0; i<ele; i++){

		if (i==index)  //if i becomes equal to index of first 0, than break loop
			break;

		if(a[i]== 0){

			for(int j=i; j<ele; j++)
				a[j]=a[j+1];

			
			a[ele-1]=0;  //assign 0 at last
			index--;     //decrease index of first 0
		}
	}
}

int main(){

	int a[] = {0, 1, 0, 3, 12, 0, 0};
	int size = sizeof(a)/sizeof(a[0]);

	move_zero_toend(a, size);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);
}
