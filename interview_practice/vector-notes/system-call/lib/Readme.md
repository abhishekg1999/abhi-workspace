## static lib

Your library contains:
mylib.a
 ├── sum.o
 │    └── sum()
 │
 └── sub.o
      └── sub()

my app : 
int main()
{
    printf("%d\n", sum(10, 20));
    return 0;
}

Static library: what gets copied?
mylib.a
 ├── sum.o  ───────────────┐
 │                         |
 └── sub.o                 |
      X                    |
      |                    |
      | not required       |
      |                    |
      v                    v
                         app
                      ┌─────────┐
main.o ─────────────> │ main()  │
                      │ sum()   │
                      │ ...     │
                      └─────────┘

## dynamic lib
mylib.so
 ├── sum.o
 │    └── sum()
 │
 └── sub.o
      └── sub()

my app :  
int main()
{
    printf("%d\n", sum(10, 20));
    return 0;
}

dynamic library: what gets copied?
                 app
        ┌──────────────────┐
        │ main()           │
        │                  │
        │ call sum() ------┼──────────┐
        │                  │          │
        └──────────────────┘          |
                                      |
                                      v
                                 mylib.so
                              ┌────────────┐
                              │ sum()      │
                              │ sub()      │
                              └────────────┘

When compiling/linking against mylib.so, the final executable contains things such as:
main()
   |
   +---- reference to "sum"
   |
   +---- information that "mylib.so" is required
   |
   +---- dynamic linking information


Runtime is where the .so comes in
1. Suppose you execute: 
	./app
2. Linux's dynamic loader sees:
	app requires:
  	mylib.so

3. It loads/maps:
	mylib.so


into the process address space look like this 
Process
┌──────────────────────────────┐
│                              │
│ app                          │
│ ┌──────────────────────────┐ │
│ │ main()                   │ │
│ │                          │ │
│ │ call sum()               │ │
│ └──────────────────────────┘ │
│             │                │
│             │                │
│             v                │
│      ┌────────────────────┐  │
│      │ mylib.so           │  │
│      │                    │  │
│      │ sum()              │  │
│      │ sub()              │  │
│      └────────────────────┘  │
│                              │
└──────────────────────────────┘

## This is the major difference static vs dynamic 

##1 static 
Before linking:

main.o              mylib.a
                     |
                     +-- sum.o
                     +-- sub.o


After linking:

              app
        ┌───────────────┐
        │ main()        │
        │ sum()         │ ← code incorporated
        └───────────────┘

mylib.a if no longer needed at runtime

what is multiple app use same static lib:
app1 → contains library code
app2 → contains library code
app3 → contains library code
app4 → contains library code
need more space 

##2 dynamic
Before linking:

main.o              mylib.so
                     |
                     +-- sum()
                     +-- sub()


After linking:

              app
        ┌───────────────┐
        │ main()        │
        │ reference     │
        │ to sum()      │
        └───────┬───────┘
                |
                v
            mylib.so
        ┌───────────────┐
        │ sum()         │
        │ sub()         │
        └───────────────┘

mylib.so is needed when the application runs.

what if multiple app use same static lib:
               mylib.so
              /   |   |   \
             /    |   |    \
          app1   app2 app3  app4
need less space


## what is dynamic loader:
The dynamic loader is the runtime component that loads an application's required shared libraries and resolves the application's references to functions/data in those libraries before or while the program runs.

             BUILD TIME																							
             ==========
               
sum.c ──> sum.o ──┐
                  │
sub.c ──> sub.o ──┤
                  │
                  v
               mylib.so
                  |
                  |
main.c ──> main.o |
                  |
                  v
              linker
                  |
                  v
                 app
                 
                 
                 
             RUN TIME
             =========

             ./app
                |
                v
       ELF program interpreter
                |
                v
       Dynamic Loader
                |
        +-------+-------+
        |               |
        v               v
    mylib.so         libc.so.6
        |               |
        |               |
        v               v
      sum()          printf()
        |               |
        +-------+-------+
                |
                v
          application runs
          
## What does "load" mean?

- This doesn't necessarily mean: copy entire .so file into RAM
- A better mental model is: The loader maps the shared library's ELF segments into the process's virtual address space.

Process virtual address space
0x00000000
+------------------+
| application      |
| app              |
+------------------+
|                  |
+------------------+
| libc.so.6        |
+------------------+
|                  |
+------------------+
| mylib.so         |
+------------------+
|                  |
+------------------+

The .so code/data becomes accessible to that process.

## types of dynamic linking

                  Dynamic linking
                       |
             +---------+---------+
             |                   |
       Automatic             Explicit
       loading               loading
             |                   |
       Dynamic loader         dlopen()
                                 |
                               dlsym()
                                 |
                              dlclose()
