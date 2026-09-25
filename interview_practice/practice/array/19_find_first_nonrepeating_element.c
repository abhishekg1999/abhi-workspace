/*
Find the first repeating element

input:
int a[] = {10, 5, 3, 4, 3, 5, 6};

output:
5

*/

#include <stdio.h>
#include <string.h>

#if 0
void find_first_nonrep(int a[], int ele){

	int seen[ele];
	memset(seen ,0, ele);

	int i,j;
	for(i=0; i<ele; i++){
	
		if(seen[i] == 1)
			continue;

		int count =1;
		for(j=i+1; j<ele; j++){
		
			if(a[i] == a[j]){

				count++;
				seen[j]=1;
			};
		}

		if(count ==1){
			printf("%d\n", a[i]);	
			break;   //break loop after find first non repeated element 
		}
	}	
}
#endif

void find_first_nonrep(int a[], int ele){


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

                if(is_dup == 0){
                        printf("%d ", a[i]);
			break;
		}
        }

        printf("\n");
}

int main(){

	int a[] = {3, 5, 3, 4, 3, 5, 6};
	int size = sizeof(a)/sizeof(a[0]);

	find_first_nonrep(a, size);
	return 0;
}
