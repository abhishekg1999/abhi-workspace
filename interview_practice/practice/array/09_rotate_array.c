/*
Rotate array by K positions
input :
int a[] = {1, 2, 3, 4, 5, 6, 7};
int k = 3;

output :
5 6 7 1 2 3 4
*/

#include <stdio.h>
void rotate_left(int a[], int ele, int rotation){

	int i, temp;

	while(rotation--){

		temp=a[0];
		for(i=0; i<ele; i++)
			a[i]=a[i+1];
		
		a[ele-1]=temp;
	}
}

void rotate_right(int a[], int ele, int rotation){

        int i, temp;

        while(rotation--){

                temp=a[ele-1];
                for(i=ele-1; i>=0; i--)
                        a[i]=a[i-1];

                a[0]=temp;
        }
}

int main(){

	int a[] = {1, 2, 3, 4, 5, 6, 7};
	int size = sizeof(a)/sizeof(a[0]);

	//rotate_left(a, size, 2);
	rotate_right(a, size, 4);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);

	printf("\n");
	return 0;
}
