
#include <stdio.h>

void remove_dup(char s[]){

	int i, j;
	int seen[256]= {0};

	for(i=0, j=0; s[i]; i++){

		int ascii = (unsigned char)s[i];
		if(seen[ascii] == 0){

			seen[ascii]=1;
			s[j++] = s[i];
		}

	}

	s[j] = '\0';
}

int main(){

	char str[] = "programming";

	printf("%s\n", str);
	remove_dup(str);

	printf("%s\n", str);
	return 0;
}
