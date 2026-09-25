/*
Find duplicate elements

input:
int a[] = {1, 2, 3, 2, 4, 5, 1};

output:
1 2

*/

#include <stdio.h>
#include <string.h>

/* 
void find_duplicate(int a[], int ele){

	int i, j;
	int seen[ele];
	memset(seen, 0, ele);

	for(i=0; i<ele; i++){

		if(seen[i] == 1)  //if element already seen , skip it 
			continue;

		int count=1;
		for(j=i+1; j<ele; j++){

			if(a[i] == a[j]){
				count++;
				seen[j]=1;   //if element seen, set 1
			}
		}	

		if(count >1)
			printf("%d ", a[i]);
	}
}
*/

void find_duplicate(int a[], int ele){


        for(int i=0; i<ele; i++){

		/* check element already printed or not, if yes than skip in "if" condition */
		int is_checked = 0;
		for(int k=0; k<i; k++){

			if(a[i] == a[k]){
				is_checked = 1;
			}
		}

		if(is_checked)
			continue;

		/* check the duplicate */
		int is_dup = 0;
		for(int j=i+1; j<ele; j++){

			if(a[i]==a[j]){
				is_dup = 1;
				break;
			}
		}

		if(is_dup == 1)
			printf("%d ", a[i]);
        }

	printf("\n");
}

int main(){

	int a[] = {0, 2, 2, 2, 3, 5, 0};
	int size = sizeof(a)/sizeof(a[0]);

	find_duplicate(a, size);
	return 0;
}
