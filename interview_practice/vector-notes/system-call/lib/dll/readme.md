########## Create Dynamic Library #############

1. Source Files
Create the following source files:
- sum.c
- sub.c
- print.c

2. Create Header File

Create a header file "mylib.h" containing function prototypes:

3. Steps to Create Static Library

Compile source files to position-independent object files:
$ cc -c -fPIC sum.c sub.c print.c

Create shared library from object files:
$ cc -shared -o mylib.so sum.o sub.o print.o

make sure to Include the header while compiling with mylib.so
#include "mylib.h"

4. Compile main.c with Static Library

$ cc main.c ./mylib.so -o main

######## or ############

you can follow the Makefile for compilation , it will create mylib.so 

