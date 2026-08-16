#include <stdio.h>

#define R 2
#define C 3

int main(){

        int a[R][C];
        int i,j;

        printf("enter the array\n");

        for(i=0; i<R; i++)
                for(j=0; j<C; j++)
                        scanf("%d", &a[i][j]);

        for(i=0; i<R; i++){
                for(j=0; j<C; j++)
                        printf("%d ", a[i][j]);
                printf("\n");
        }

        return 0;
}
~
