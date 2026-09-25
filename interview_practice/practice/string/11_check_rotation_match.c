/*
String rotation

input:
char a[] = "abcd";
char b[] = "cdab";

are rotations of each other.
output:
Yes
*/

#include <stdio.h>
#include <string.h>

int check_match(char a[], char b[]){

	int len = strlen(a);
	if(strlen(a) != strlen(b))
		return -1;

	for(int i=0; a[i]; i++){

		/* rotation */
		char temp=a[0];
		for(int j=0; a[j]; j++)
			a[j]=a[j+1];

		a[len-1]= temp;

		/* check after every rotation */
		if( strcmp(a, b) == 0)
			return 0;  //matched
	} 

	return -1;  //not matched
}

int main(){

	char a[] = "abcd";
	char b[] = "cdba";

	int ret = check_match(a, b);
	if(ret == 0)
		printf("macthed\n");
	else
		printf("not mached\n");

	return 0;
}
