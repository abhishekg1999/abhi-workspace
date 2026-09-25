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
#include <string.h>

#if 0
void find_duplicate(char s[]){

	int size = strlen(s)-1;
	int seen[size];
	memset(seen, 0, size*sizeof(int));

	for(int i=0; s[i]; i++){
		
		if(seen[i]==1)
			continue;

		int count=1;
		for(int j=i+1; s[j]; j++){

			if(s[i]==s[j]){
				count++;
				seen[j]=1;
			}
		}

		if(count>1)
			printf("%c\n", s[i]);
	}
}

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

void find_duplicate(char s[]){


        for(int i=0; s[i]; i++){

                /* check element already printed or not, if yes than skip in "if" condition */
                int is_checked = 0;
                for(int k=0; k<i; k++) {

                        if(s[i] == s[k]){
                                is_checked = 1;
                        }
                }

                if(is_checked)
                        continue;
		
		/* check the duplicate */
                int is_dup = 0;
                for(int j=i+1; s[j]; j++){

                        if(s[i]==s[j]){
                                is_dup = 1;
                                break;
                        }
                }

                if(is_dup == 1)
                        printf("%c ", s[i]);

        }

        printf("\n");
}

int main(){

	char str[] = "programming";

	find_duplicate(str);

	return 0;
}
