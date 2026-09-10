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

        int i;
        int count[256]={0};  // Array to hold the count of all 256 possible ASCII characters

        /* count many times each character appear */
        for(i=0; i<ele; i++){

                int val = a[i];  //store each character ascii
                count[val]++;   //increment count for every ascii
        }

        for(i=0; i<ele; i++){

                int val = a[i];
                if(count[val] > 1) {

                        printf("%d \n", a[i]);
                        count[val] = 0;   //make it 0, dont print same character freq again
                }
        }
}


int main(){

	int a[] = {1, 2, 3, 2, 4, 5, 1};
	int size = sizeof(a)/sizeof(a[0]);

	find_duplicate(a, size);
	return 0;
}
