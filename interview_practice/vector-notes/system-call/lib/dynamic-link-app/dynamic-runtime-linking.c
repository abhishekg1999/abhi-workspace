#include <stdio.h>
#include <dlfcn.h>
// #include "mylib.h"  //optional because loading library at run time

int main(){

        int a=10, b=20, ret, op;
        void *handler;
        int (*fptr)(int, int);

        printf("enter the option: 1)sum 2)sub\n");
        scanf("%d",&op);

        handler = dlopen("./mylib.so", RTLD_LAZY); // open/loading dynamic library to RAM
        if(handler == 0){
                printf("%s\n",dlerror());  //print dl error msg
                return 1;
        }

        switch(op) {

                case 1:
                        fptr=dlsym(handler, "my_sum"); //resolve the symbol adress and return to fptr
                        if(fptr ==0){
                                printf("%s\n",dlerror());  //print dl error msg
                                return 1;
                        }

                        ret = fptr(a, b);
                        printf("sum=%d\n",ret);
                        break;

                case 2:
                        fptr=dlsym(handler, "my_sub");
                        if(fptr ==0){
                                printf("%s\n",dlerror());  //print dl error msg
                                return 1;
                        }

                        ret = fptr(a, b);
                        printf("sub=%d\n",ret);
                        break;

                default:
                        printf("unknown option\n");
        }

        dlclose(handler); //unload dl from RAM
        return 0;

}
