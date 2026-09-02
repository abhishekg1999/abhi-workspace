/*check signatures of two numbers are same or not */

#include <stdio.h>
int sign_check(int a, int b){
        if( (a>0 && b<0) || (a<0 && b>0) )
                return 1;
        else
                return 0;
}

int main(){

        int ret;
        ret =sign_check(1, 1);
        ret ? printf("sign is different\n") : printf("sign is same\n");
        return 0;
}

