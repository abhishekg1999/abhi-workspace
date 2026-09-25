
#include <stdio.h>


void remove_dup(char s[]){

        for(int i=0; s[i]; i++){

                for(int j=i+1; s[j]; j++){

                        if(s[i]==s[j]){

                                for(int k=j; s[k]; k++)
                                        s[k]=s[k+1];
                                i--;
                        }
                }
        }
}

#if 0
void remove_dup(char s[]){

	int i, j;
	int seen[256]= {0};

	for(i=0, j=0; s[i]; i++){

		int ascii = (unsigned char)s[i];
		if(seen[ascii] == 0){

			s[j++] = s[i];
			seen[ascii]=1;
		}

	}

	s[j] = '\0';
}
#endif

int main(){

	char str[] = "programming";

	printf("%s\n", str);
	remove_dup(str);

	printf("%s\n", str);
	return 0;
}
