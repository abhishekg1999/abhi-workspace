#include <stdio.h>

#define R 5
#define C 10

int main(){

        char a[R][C];
        int i,ele;
        ele= sizeof(a)/sizeof(a[0]);

        printf("enter the array\n");
        for(i=0; i<ele; i++)
                scanf("%s", a[i]);

        for(i=0; i<ele; i++)
                printf("%s ", a[i]);
        printf("\n");
        return 0;
}
