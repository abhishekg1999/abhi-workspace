/*
First non-repeating character

input:
char str[] = "swiss";

output:
s
*/

#include <stdio.h>
#include <string.h>

void find_rep(char s[]){

	int size = strlen(s);
	int seen[size];

	memset(seen, 0, size);

	for(int i=0; s[i]; i++){

		if(seen[i] ==1)
			continue;

		int count=1;
		for(int j=i+1; s[j]; j++){

			if(s[i] == s[j]){
				count++;
				seen[j]=1;
			}

		}

		if(count >1){
			printf("%c\n", s[i]);
			break;
		}
	}

}

/*
void find_rep(char s[]){

	int count[256] = {0};
	int i;

	for(i=0; s[i]; i++){

		int ascii = (unsigned char)s[i];
		count[ascii]++;
	}

	for(i=0; s[i]; i++){

		int ascii = (unsigned char)s[i];
		if(count[ascii] > 1){
			printf("%c\n", s[i]);
			break;
		}
	}
}

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

		if(is_repeated ==0) {
			printf("%c\n", s[i]);
			break;
		}
	}
}
*/

int main(){

	char str[] = "swiss";

	find_rep(str);
	return 0;
}
