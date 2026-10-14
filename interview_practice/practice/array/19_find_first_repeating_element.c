/*
Find the first repeating element

input:
int a[] = {10, 5, 3, 4, 3, 5, 6};

output:
3

*/

#include <stdio.h>
#include <string.h>

void find_first_rep(int a[], int ele){


        for(int i=0; i<ele; i++){

                /* check element already printed or not, if yes than skip in "if" condition */
                int is_checked = 0;
                for(int j=0; j<i; j++){

                        if(a[i] == a[j]){
                                is_checked = 1;
                        }
                }

                if(is_checked) {
                        printf("%d\n", a[i]);
                        break;
		}
	}
}

int main(){

	int a[] = {10, 5, 3, 4, 3, 5, 6};
	int size = sizeof(a)/sizeof(a[0]);
	
	find_first_rep(a, size);
	return 0;
}
