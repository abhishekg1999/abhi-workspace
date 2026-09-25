/*
Count vowels and consonants

input:
char str[] = "Embedded System";

output:
vowels count ?
consonants count ?

*/

#include <stdio.h>

void count_vowel_conso(char s[]){

	int vow=0;
	int cons=0;
	char ch;

	for(int i=0; s[i]; i++){
	
		/* convert upper case to lower */	
		if(s[i] >= 'A' && s[i]<= 'Z')
			ch = s[i]+32;
		else
			ch = s[i];

		/* check the vowels and consonants */
		if( ch>= 'a' && ch <= 'z'){
		   
		   if( ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
			   vow++;
	           else
			   cons++;
		}
	}

	printf("vow =%d cons =%d\n", vow, cons);
}

int main(){

	char s[] = "Embedded Sys****tem";
	count_vowel_conso(s);

	return 0;
}
