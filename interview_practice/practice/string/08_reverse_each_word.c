/*
Reverse each word

input:
char str[] = "I love embedded";

output:
I evol deddebme

*/

#include <stdio.h>
#include <string.h>

void my_strrev(char s[]){

	int i,j;
	char temp;

	for(j=0; s[j]; j++);
	for(i=0, j=j-1; i<j; i++, j--) {

		temp=s[i];
		s[i]=s[j];
		s[j]=temp;
	}
}

void my_strrev1(char *p,  char *q){

	char temp;
	while(p<q){

		temp=*p;
		*p=*q;
		*q=temp;
	
		p++;
		q--;
	}
}

void rev_word(char s[]){

	char *p, *q;

	p=s;
	while( q=strchr(p, ' ')){

		my_strrev1(p, q-1);
		p=q+1;
	}

	my_strrev(p);
}
	
int main(){

	char str[] = "I love embedded";

	printf("%s\n", str);
	rev_word(str);

	printf("%s\n", str);
	return 0;
}
