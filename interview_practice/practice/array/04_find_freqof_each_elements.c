/*
Find frequency of each element
intput:
int a[] = {1, 2, 2, 3, 1, 2};

output:
1 -> 2
2 -> 3
3 -> 1

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
			continue;   //if element already seen, than skip loop

		int count=1;
		for(j=i+1; j<ele; j++){
			if(a[i] == a[j]) {
				seen[j]=1;   //set 1 , if element seen
				count++;
			}
		}

		printf("%d => %d\n", a[i], count);
	}
}

int main(){

	int a[] = {1, 2, 1, 3, 1, 2};
	int size = sizeof(a)/sizeof(a[0]);

	array_freq(a, size);	
	return 0;
}
