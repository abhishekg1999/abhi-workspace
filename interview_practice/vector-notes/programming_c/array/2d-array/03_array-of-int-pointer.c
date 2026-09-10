#include <stdio.h>
#include <limits.h>

#define r 2
#define c 3

int main(){
	
	int a[r][c] = {{1,2,3},{4,5,6}};
	//int *p[r] = {{1,2,3},{4,5,6}};   // incorrect
	
	int *p[r];
	p[0]=a[0];
	p[1]=a[1];
	
	for(int i=0; i<r; i++){
	for(int j=0; j<c; j++)
		printf("%d ", p[i][j]);
		printf("\n");
	}

	printf("\n");
	return 0;
} 
