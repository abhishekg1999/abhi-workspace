#include <stdio.h>
#include <stdlib.h>

#define r 2
#define c 3

int main()
{
	int i,j;

	//int *p[r]; //allocate mr for array of pointer statically at compile time
	
	/* or */	
	int **p;
	p=malloc(sizeof(int*)*r); //allocate mr for array of pointer at run time
	
	for(i=0;i<r;i++)
		p[i]=malloc(sizeof(int)*c);

	printf("enter the elements of array\n");
	for(i=0;i<r;i++)
		for(j=0;j<c;j++)
			scanf("%d",&p[i][j]);

	printf("array of integer\n");
	for(i=0;i<r;i++)
	{
		for(j=0;j<c;j++)
			printf("%d ",p[i][j]);

			printf("\n");
	}

	/* free each pointer in double */
	for(i = 0; i < r; i++)
    		free(p[i]);  
      	/* free double pointer */	
	free(p);

}
	

/* allocation of memory */
        p  (int **)
        │
        ├───────────────┐
        │               │
      p[0]            p[1]
       │                │
   ┌───┴───┬───┬───┐  ┌───┴───┬───┬───┐
   │int    │int│int│  │int    │int│int│
   │[0][0] │   │   │  │[1][0] │   │   │
   └───────┴───┴───┘  └───────┴───┴───┘
