## APIS 

void *handler = dlopen("./mylib.so", RTLD_LAZY); //open/loading dynamic library to RAM
int (*fptr)(int, int);
fptr=dlsym(handler, "my_sum");  //resolve the symbol adress and return to fptr
dlclose(handler); //unload dl from RAM  


