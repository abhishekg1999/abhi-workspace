/*
First non-repeating character

input:
char str[] = "swiss";

output:
w
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void find_rep(char s[]){

	int i,j;
	for(i=0; s[i]; i++){

		int is_repeated = 0;
		for(j=0; s[j]; j++){

			if( s[i] == s[j] && i != j){
				is_repeated =1;
				break;
			}
		}

		if(is_repeated ==1) {
			printf("%c\n", s[i]);
			break;
		}
	}
}

int main(){

	char str[] = "swiss";

	find_rep(str);
	return 0;
}
