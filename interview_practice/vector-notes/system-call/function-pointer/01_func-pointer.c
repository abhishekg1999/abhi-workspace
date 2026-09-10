#include <stdio.h>

int sum(int a, int b){
        return a+b;
}

int main(){

        int ret;
        int (*fptr)(int, int);

        fptr=sum;
        ret = fptr(10, 20);
        printf("sum =%d\n", ret);
        return 0;
}

