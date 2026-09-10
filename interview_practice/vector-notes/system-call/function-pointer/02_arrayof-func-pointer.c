#include <stdio.h>

typedef int(*fptr)(int , int);
int sum(int a, int b){
        return a+b;
}

int sub(int a, int b){
        return a-b;
}

int main(){

        int a=10, b=20, op;

        fptr ptr[2];
        ptr[0] =sum;
        ptr[1] =sub;

        /*  or  */
        /*
        fptr ptr[2] = {
                sum,
                sub
        }; */

        printf("enter  option 1)sum 2)sub:\n");
        scanf("%d",&op);

        if(op>=1 && op<=2)
                printf("%d\n", ptr[op-1](a, b));
        else
                printf("unknown option:\n");

        return 0;
}
