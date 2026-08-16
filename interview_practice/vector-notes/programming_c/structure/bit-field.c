#include <stdio.h>
#include <stdlib.h>

typedef struct {
        char ch;
        unsigned int bit:3;
}student;

int main(){
        student temp = {
                .ch= 'a',
                .bit=7
        };

        printf("char =%c bit=%d\n", temp.ch, temp.bit);
        return 0;
}

