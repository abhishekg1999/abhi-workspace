/*
Replace consecutive spaces

input:
char str[] = "hello    embedded     world";

output:
hello embedded world

*/

#include <stdio.h>

void remove_cons_space(char a[]){

	int i,j;

	for(i=0; a[i]; i++){

		if(a[i] == ' '){

			/* check if space availble on i+1 also, delete all space after i */
			if(a[i+1] == ' '){
			   
				for(j=i+1; a[j]; j++)
				   a[j]=a[j+1];

				i--;  //decrease for next consicutive 
			}
		}
	}

	/* enable this for string b[]
	for(i=0; a[i]; i++)
		a[i]=a[i+1];
	*/
}

int main(){

	char a[] = "hello    embedded     world";
	//char b[] = "  Embedded   Linux  Developer  ";

	remove_cons_space(a);
	printf("%s\n", a);

	return 0;
}
