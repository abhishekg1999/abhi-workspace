#include <stdio.h>

typedef union {
        int i;
        float f;
}temp;

int main(){
        temp x;
        x.f =23.5;

        for(int pos=31; pos>=0; pos--)
                printf("%d", (x.i>>pos)&1);

        printf("\n");
        return 0;
}
