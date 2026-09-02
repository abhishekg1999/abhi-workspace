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

	int i;
	int count1=0, count2=0;
	char ch;

	for(i=0; s[i]; i++){
	
		/* convert upper case to lower */	
		if(s[i] >= 'A' && s[i]<= 'Z')
			ch = s[i]+32;
		else
			ch = s[i];

		/* check the vowels and consonants */
		if( ch>= 'a' && ch <= 'z'){
		   
		   if( ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
			   count1++;
	           else
			   count2++;
		}
	}

	printf("vow =%d cons =%d\n", count1, count2);
}

int main(){

	char s[] = "Embedded Sys****tem";
	count_vowel_conso(s);

	return 0;
}
