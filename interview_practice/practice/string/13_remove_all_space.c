/*
Replace consecutive spaces

input:
char str[] = "hello    embedded     world";

output:
helloembeddedworld

*/

#include <stdio.h>

void remove_cons_space(char a[]){

	int i,j;

	for(i=0; a[i]; i++){

		if(a[i] == ' '){

			/* remove space */
			for(j=i; a[j]; j++)
			   a[j]=a[j+1];

			i--;  //decrease for next consicutive space
		}
	}
}

int main(){

	char a[] = "hello    embedded     world";
	//char b[] = "  Embedded   Linux  Developer  ";

	remove_cons_space(a);
	printf("%s\n", a);

	return 0;
}
