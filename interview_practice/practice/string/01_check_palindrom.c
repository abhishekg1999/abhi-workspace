/*
Check palindrome

input:
char str[] = "madam";

Output:
Palindrome
*/

#include <stdio.h>
#include <string.h>

int find_palindrom(char s[]){

	int i;
	int j= strlen(s)-1;

	for(i=0; i<j; i++, j--){

		if(s[i] != s[j])
			return 0;   //not palindrom, find not matching character
	}

	return 1;  //pelindrom, return 1
}

int main(){

	char s[] = "madam";
	int ret = find_palindrom(s);

	if(ret)
		printf("palindrom\n");
	else
		printf("not palindrom\n");

	return 0;
}
