#include <stdio.h>
#include <stdlib.h>

#define r 2
#define c 10

int main()
{
	char **p;
	int i;

	//allocate memory for array of char pointers
	p=malloc(sizeof(char*)*r);
	
	//each pointer pointing to allocated memory
	for(i=0;i<r;i++)
		p[i]=malloc(sizeof(char)*c);


	//scan value
	printf("enter the strings \n");
	for(i=0;i<r;i++)
		scanf("%s",p[i]);

	//print value
	printf("*****strings****** \n");
	for(i=0;i<r;i++)
		printf("%s\n",p[i]);

	for(i=0; i<r; i++)
                free(p[i]);
        free(p);

	return 0;
	
}
