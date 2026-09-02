#include <stdio.h>
#include <limits.h>

#define r 2
#define c 3

void printarray1(int *b[r]){
	
	for(int i=0; i<r; i++){
	for(int j=0; j<c; j++)
		printf("%d ", b[i][j]);
		printf("\n");
	}
}

void printarray2(int (*b)[c]){
	
	for(int i=0; i<r; i++){
	for(int j=0; j<c; j++)
		printf("%d ", b[i][j]);
		printf("\n");
	}
}

void printarray3(int **b){

	for(int i=0; i<r; i++){
	for(int j=0; j<c; j++)
		printf("%d ", b[i][j]);
		printf("\n");
	}
}

int main(){
	
	/* way 1 */
	//int a[r][c] = {{1,2,3},{4,5,6}};
	//printarray1(a);  /* error */
	//printarray2(a);    /* ok */
	//printarray3(a);    /* error */
	
	/* way 2 */
	//int *a[3] = {{10,20,30}, {40,50,60}};  /* incorrect */
	
	/* way 3 */
	int a[r][c] = {{1,2,3},{4,5,6}};
	int *p[2];
	p[0]=a[0];
	p[1]=a[1]; 
	
	//printarray1(p);  /* ok  */
	//printarray2(p);    /* error */
	//printarray3(p);  /* ok */
		
	printf("\n");
	return 0;
}
