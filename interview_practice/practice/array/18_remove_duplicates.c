/*
Remove duplicates from a sorted array

input:
int a[] = {1, 1, 2, 2, 2, 3, 4, 4, 5};

output:
Modify the same array so that the unique elements are at the beginning:
1 2 3 4 5

*/

#include <stdio.h>

#if 0
void remove_dup(int a[], int *ele){

	int i, j, k;
	for(i=0; i<(*ele); i++){

		for(j=i+1; j<(*ele); j++){

			if(a[i] == a[j]){
				
				for(k=j; k<(*ele); k++)
					a[k]=a[k+1];
				
				j--;
				(*ele)--;
			}
		}
	}
}
#endif 

void remove_dup(int a[], int *ele){


	int j = 0;
	for(int i=0; i<*ele; i++){

                /* check element match, if yes than skip in "if" condition */
                int is_matched = 0;
                for(int k=0; k<i; k++){

                        if(a[i] == a[k]){
                                is_matched = 1;
                        }
                }

                if(is_matched)
                        continue;

		if(is_matched == 0)
			a[j++] = a[i];

	}

	*ele = j;  //update new size after remove duplicate
}

int main(){

	int a[] = {1, 1, 2, 2, 1, 3, 4, 4, 5};
	int size = sizeof(a)/sizeof(a[0]);

	remove_dup(a, &size);
	for(int i=0; i<size; i++)
		printf("%d ", a[i]);
	
	printf("\n");
	return 0;
}
