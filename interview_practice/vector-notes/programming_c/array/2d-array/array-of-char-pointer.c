#include <stdio.h>
#include <limits.h>

#define r 2
#define c 10

int main(){
	
	char a[r][c] = {"abcd","efgh"};
	char *p[r] = {"hello","bye"};   /* correct */
	
	/*
	char *p[r];
	
	p[0]=a[0];
	p[1]=a[1];*/
	
	for(int i=0; i<r; i++){
		printf("%s ", p[i]);
		printf("\n");
	}
	
	printf("\n");
	return 0;
}
