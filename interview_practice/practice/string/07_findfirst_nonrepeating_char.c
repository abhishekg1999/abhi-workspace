/*
First non-repeating character

input:
char str[] = "swiss";

output:
w
*/

#include <stdio.h>

void find_nonrep(char s[]){

	int i,j;

	for(i=0; s[i]; i++){

		int is_repeated = 0;
		for(j=0; s[j]; j++){

			if( i!=j && s[i] == s[j]){
				is_repeated =1;
				break;
			}
		}

		if(is_repeated ==0) {
			printf("%c\n", s[i]);
			break;
		}
	}
}

int main(){

	char str[] = "swiss";

	find_nonrep(str);
	return 0;
}
