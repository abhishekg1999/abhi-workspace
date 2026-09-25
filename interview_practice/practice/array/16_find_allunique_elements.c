/*
Find only unique elements
intput:
int a[] = {1, 29, 1, 3, 1, 2};

output:
29, 3, 2

*/

#include <stdio.h>
#include <string.h>

#if 0
void array_freq(int a[], int ele){

	/* take flag to monitor seen or unseen elements */
	int seen[ele];
	memset(seen, 0, ele*sizeof(int));

	for(int i=0; i<ele; i++) {
		
		if(seen[i]==1)
			continue;   //if element already seen, than skip that element

		int count=1;
		for(int j=i+1; j<ele; j++){
			if(a[i] == a[j]) {
				count++;
				seen[j]=1;   //set 1 , if element seen
			}
		}
		
		if(count ==1)
			printf("%d ", a[i]);
	}
}
#endif

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

                if(is_dup == 0)
                        printf("%d ", a[i]);
        }

        printf("\n");
}

int main(){

	int a[] = {1, 29, 1, 3, 1, 2};
	int size = sizeof(a)/sizeof(a[0]);

	array_freq(a, size);	
	printf("\n");
	return 0;
}
