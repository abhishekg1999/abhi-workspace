#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>

int main(){

	int j;
	char *p;

	printf("mtrace started:\n");
	mtrace();   //start tracing
	p=malloc(16);
	free(p);
	p=calloc(2, 10);

	muntrace();  //stop tracing
	return 0;
}
