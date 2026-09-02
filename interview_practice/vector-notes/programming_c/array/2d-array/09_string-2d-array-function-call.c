#include <stdio.h>
#include <limits.h>

#define r 2
#define c 10

void printarray1(char *b[r]){
	
	for(int i=0; i<r; i++){
		printf("%s ", b[i]);
		printf("\n");
	}
}

void printarray2(char (*b)[c]){
	
	for(int i=0; i<r; i++){
		printf("%s ", b[i]);
		printf("\n");
	}
}

void printarray3(char **b){

	for(int i=0; i<r; i++){
		printf("%s ", b[i]);
		printf("\n");
	}
}

int main(){
	
	/* way 1 */
	//char a[r][c] = {"abcd","efgh"};
	//printarray1(a);  /* error */
	//printarray2(a);    /* ok */
	//printarray3(a);    /* error */
	
	/* way 2 */
	//char *a[2] = {"abcd", "efgh"}; /* correct */
	//printarray1(a);  /* ok */
	//printarray2(a);    /* error */
	//printarray3(a);    /* ok */
	
	/* way 3 */
	char a[r][c] = {"abcd","efgh"};
	char *p[2];
	p[0]=a[0];
	p[1]=a[1]; 
	
	//printarray1(p);  /* ok  */
	//printarray2(p);    /* error */
	//printarray3(p);  /* ok */
		
	printf("\n");
	return 0;
}
