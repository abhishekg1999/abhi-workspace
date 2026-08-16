#include <stdio.h>
typedef struct {
        unsigned int i;
        char ch;
        float f;
}student;

int main(){
        student s[2]={0};
        student *p = s;

        int size = (char*)(p+1) - (char*)p;
        printf("%d\n",size);

        return 0;
}

