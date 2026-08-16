#include <stdio.h>

typedef int(*fptr)(int , int);

int sum(int a, int b){
        return a+b;
}

int call_back_func(int a, int b, fptr ptr){
        int ret;
        ret = ptr(a, b);

        return ret;
}

int main(){

        int ret;

        ret = call_back_func(10, 20, sum);

        printf("sum =%d\n", ret);
        return 0;
}
