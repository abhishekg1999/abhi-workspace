/*
Find only unique elements
intput:
int a[] = {1, 29, 1, 3, 1, 2};

output:
29, 3, 2

*/

#include <stdio.h>
void array_freq(int a[], int ele){

	int i, j;

	/* take flag to monitor seen or unseen elements */
	int seen[ele];
	for(i=0; i<ele; i++)
		seen[i]=0;    

	for(i=0; i<ele; i++) {
		
		if(seen[i]==1)
			continue;   //if element already seen, than skip that element

		int count=1;
		for(j=i+1; j<ele; j++){
			if(a[i] == a[j]) {
				count++;
				seen[j]=1;   //set 1 , if element seen
			}
		}
		
		if(count ==1)
			printf("%d ", a[i]);
	}
}

int main(){

	int a[] = {1, 29, 1, 3, 1, 2};
	int size = sizeof(a)/sizeof(a[0]);

	array_freq(a, size);	
	printf("\n");
	return 0;
}
