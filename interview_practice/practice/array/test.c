#include <stdio.h>
#include <string.h>
#include <limits.h>

int fun(char a[], char b[]){

	int count[256] = {0};
	if(strlen(a) != strlen(b))
		return -1;

	for(int i=0; a[i]; i++){

		int ascii_a = (unsigned char)a[i];
		int ascii_b = (unsigned char)b[i];

		count[ascii_a]++;
		count[ascii_b]--;
	}

	for(int i =0; i<256; i++){

		if(count[i] !=0)
			return -1;
	}

	return 0;
}

#if 0
void fun(char s[]){

	for(int i=0; s[i]; i++){

		if(s[i] == ' '){
			for(int j=i; s[j]; j++)
				s[j]=s[j+1];
			i--;
		}
	}
}

void fun(char s[]){

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

void fun(char s[]){

	int size = strlen(s);
	int seen[size];
	memset(seen, 0, size);

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
		printf("%c %d\n", s[i], count);
	}
}

void fun(char s[]){

	int i,j;
	int seen[256] = {0};


	for(i=0, j=0; s[i]; i++){
		
		int ascii = (unsigned char)s[i];
		if(seen[ascii] == 0){

			s[j++] = s[i];
			seen[ascii] = 1;
		}
	}

	s[j] = '\0';
}
#endif

int main(){

	char str1[] = "silent";
	char str2[] = "listen";
	int ret = fun(str1, str2);

	printf("%d\n", ret);
	printf("\n");
	return 0;
}

