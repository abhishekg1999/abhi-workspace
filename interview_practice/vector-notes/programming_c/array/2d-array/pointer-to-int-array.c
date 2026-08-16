#include <stdio.h>

#define r 2
#define c 3

void print_array(int (*p)[3],int row,int col) {
        
	int i,j;
        for(i=0;i<row;i++) {
           for(j=0;j<col;j++)
              printf("%d ",p[i][j]);

           printf("\n");
        }
}

int main()
{
	int b[r][c]={10,20,30,40,50,60};
	print_array(b,r,c);
}
