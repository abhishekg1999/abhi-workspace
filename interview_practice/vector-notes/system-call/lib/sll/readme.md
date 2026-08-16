########## Create Static Library #############

1. Source Files
Create the following source files:
- sum.c
- sub.c
- print.c

3. Create Header File

Create a header file "mylib.h" containing function prototypes:

2. Steps to Create Static Library

Compile source files to object files:
$ cc -c sum.c sub.c print.c

Create static library from object files:
$ ar -rcs mylib.a *.o

make sure to Include the header while compiling with mylib.a
#include "mylib.h"

4. Compile main.c with Static Library

Static linking:
$ cc main.c --static ./mylib.a -o main

Normal linking using static library:
$ cc main.c ./mylib.a -o main

5. Useful ar Command Options

List object files inside library:
$ ar -t mylib.a

Delete an object file from library:
$ ar -d mylib.a sum.o

Add or replace an object file in library:
$ ar -r mylib.a sum.o

####### or ###########

you can go for Makefile , every steps mentioned in MAkefile to generate static library "mylib.a"
