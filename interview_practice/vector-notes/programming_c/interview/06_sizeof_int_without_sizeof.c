#include <stdio.h>
int main()
{
        int a;
        int *p=&a;
        int size=(char*)(p+1)-(char*)p;
        printf("integer size=%d\n",size);
}

