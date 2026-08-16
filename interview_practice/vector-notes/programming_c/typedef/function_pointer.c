#include <stdio.h>

typedef int (*FPTR)(int,int);

int my_add(int a, int b){
        return a+b;
}

int main(){

        FPTR fp = my_add;
        int result = fp(10, 20);

        printf("result =%d\n", result);
        return 0;
}
