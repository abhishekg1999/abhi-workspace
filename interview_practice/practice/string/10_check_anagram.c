/*
Check whether two strings are anagrams

input:
char a[] = "listen";
char b[] = "silent";

output:
Anagram

*/

#include <stdio.h>
#include <string.h>

int check_anagram(char a[], char b[]){

	int i;
	int count[256] = {0};

	/* if both string length not equal , not anagram */
	if( strlen(a) != strlen(b))
		return -1;

	for(i=0; a[i]; i++){

		int ascii_a = (unsigned char)a[i];
		int ascii_b = (unsigned char)b[i];

		count[ascii_a]++;  //add 1 for one character from string a  (like a=1)
		count[ascii_b]--;  //subtract 1 for same character from string b   (like a=0)
	}

	/* Check if all slots returned back to zero */
	for(i=0; i<256; i++){

		if(count[i] !=0)
			return -1;   //a non-zero found, not anagram
	}

	return 0;
}

int main(){

	char a[] = "listen";
	char b[] = "silent";

	int ret = check_anagram(a, b);
	if(ret ==0)
		printf("anagram \n");
	else
		printf("not anagram\n");

	return 0;
}
