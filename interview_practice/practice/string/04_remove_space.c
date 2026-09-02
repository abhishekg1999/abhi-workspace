/*
Remove spaces

input:
char str[] = "Embedded Linux Developer";

output:
EmbeddedLinuxDeveloper

*/

#include <stdio.h>

void remove_space(char s[]){

	int i,j;

	for(i=0; s[i]; i++){

		if(s[i] == ' ')
		  for(j=i; s[j]; j++)
			  s[j]=s[j+1];
	}

}

int main(){

	char str[] = "Embedded Linux Developer";

	printf("%s\n", str);
	remove_space(str);

	printf("%s\n", str);
	return 0;
}
