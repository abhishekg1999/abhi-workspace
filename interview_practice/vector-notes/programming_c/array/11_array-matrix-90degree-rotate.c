/*
 * rotate matrix 90 degree left & 90 degree right
 */

#include <stdio.h>

#define r 2
#define c 3

void print_array(int row, int col, int b[row][col]) {

	for(int i=0;i<row;i++) {
	   for(int j=0; j<col; j++)
	      printf("%d ",b[i][j]);
	      printf("\n");
	}
}

int main() {

	int a[r][c] = {{1,2,3},{4,5,6}};
	int b[c][r], i ,j;

	print_array(r,c,a);
	/* 90 degree left */
	for(i=0; i<c; i++)
	   for(j=0; j<r; j++)
   	      b[i][j]= a[j][c-1-i];
	
	
	/* 90 degree right 
	for(i=0; i<c; i++)
	   for(j=0; j<r; j++)
  	      b[i][j] = a[r-1-j][i];

	*/
	printf("\n");
	print_array(c,r,b);
	printf("\n");	
	return 0;
}
