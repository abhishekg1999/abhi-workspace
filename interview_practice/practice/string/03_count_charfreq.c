/*
Count character frequency

input:
char str[] = "hello world";

output:
h -> 1
e -> 1
l -> 3
o -> 2

*/

#include <stdio.h>
#include <string.h>

void count_freq(char s[]){

	int i,j;
	int size = strlen(s);
	int seen[size];

	for(i=0; i<size; i++)
		seen[i]=0;
	
	for(i=0; s[i]; i++){

		if(seen[i] ==1 || s[i] == ' ')  //skip for seen character and space
			continue;

		int count=1;
		for(j=i+1; s[j]; j++){
			
			if(s[i] == s[j]){
				count++;
				seen[j]=1;
			}
		}
		
		printf("%c => %d\n", s[i], count);
	}
}

int main(){

	char str[] = "hello world";
	count_freq(str);

	return 0;
}

