#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>


int* my_fun(void){

        int i=10;
        return &i;
}

int main(){

        int *ptr;

        ptr = my_fun();
        printf("ptr =%d\n", *ptr); //segmentation fault, ptr try to access randome address
        printf("\n");
        return 0;
}
