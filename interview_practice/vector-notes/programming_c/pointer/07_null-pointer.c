#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>


int main(){

        int *ptr = NULL;

        printf("%lu\n", ptr);
        //printf("%d\n", *ptr); //error , segmentation fault
        printf("\n");
        return 0;
}
