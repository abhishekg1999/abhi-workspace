/* 
 * same dimension matrix 
 *
*/`

#include <stdio.h>

#define r 3
#define c 3
void printarray(int a[r][c]){

	for(int i=0; i<r; i++){
	for(int j=0; j<c; j++)
		printf("%d ", a[i][j]);

		printf("\n");
	}
}

int main(){


	int a[r][c]= {{1,2,3},{4,5,6},{7,8,9}};
	int b[r][c];
	int i,j;

	printarray(a);
	/* 90 degree left
	for(i=0; i<c; i++)
	for(j=0; j<r; j++)
		b[i][j]=a[j][c-1-i];
	*/

	/* 90 degree right */
	for(i=0; i<c; i++)
	for(j=0; j<r; j++)
		b[i][j]=a[r-1-j][i];

	printarray(b);

	return 0;
}
