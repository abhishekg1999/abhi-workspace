/*
Find duplicate elements

input:
int a[] = {1, 2, 3, 2, 4, 5, 1};

output:
1 2

*/

#include <stdio.h>
#include <string.h>

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

int main(){

	int a[] = {1, 2, 3, 2, 4, 5, 1};
	int size = sizeof(a)/sizeof(a[0]);

	find_duplicate(a, size);
	return 0;
}
