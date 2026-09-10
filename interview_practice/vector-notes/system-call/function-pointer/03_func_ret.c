#include <stdio.h>

typedef int(*fptr)(int , int);

int sum(int a, int b){
        return a+b;
}

fptr func_1(void) {
        return sum;
}

int main(){

        int ret;

        fptr ptr = func_1();
        ret = ptr(10, 20);

        printf("sum =%d\n", ret);
        return 0;
}
