#include <stdio.h>

typedef union {
        char ch;
        int i;
}student;

int main(){
        student x;
        x.i = 0x12345678;
        printf("ch =0x%x i=0x%x\n", x.ch, x.i);
        if(x.ch == 0x78)
                printf("Little\n");
        else
                printf("Big\n");
        return 0;
}
