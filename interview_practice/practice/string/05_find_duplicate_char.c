/*
Find duplicate characters

input:
char str[] = "programming";

output:
r
g
m

*/

#include <stdio.h>

void find_duplicate(char s[]){

        int count[256] = {0};
        for(int i=0; s[i]; i++){

                int ascii = (unsigned char)s[i];
                count[ascii]++;
        }

        for(int i=0; s[i]; i++){

                int ascii = (unsigned char)s[i];
                if(count[ascii] ==1){

                        printf("%c ", s[i]);
                        count[ascii] = 0;
                }
        }
}

#if 0
void find_duplicate(char s[]){

	int i;
	int count[256]={0};  // Array to hold the count of all 256 possible ASCII characters

	/* count many times each character appear */
	for(i=0; s[i]; i++){

		int ascii = (unsigned char)s[i];  //store each character ascii
		count[ascii]++;   //increment count for every ascii
	}

	for(i=0; s[i]; i++){

		int ascii = (unsigned char)s[i];
		if(count[ascii] > 1) {
			
			printf("%c \n", s[i]);
			count[ascii] = 0;   //make it 0, dont print same character freq again 
		}
	}
}
#endif

int main(){

	char str[] = "programming";

	find_duplicate(str);

	return 0;
}
