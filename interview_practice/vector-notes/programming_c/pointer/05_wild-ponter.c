#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>


int main(){

        int *ptr;

        printf("%lu\n", ptr);  //invalid address
        printf("%d\n", *ptr);  //garbage value
        printf("\n");
        return 0;
}
