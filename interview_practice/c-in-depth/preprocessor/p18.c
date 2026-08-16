#include <stdlib.h>
#include <stdio.h>
#define PRINT1(message) printf(message);
#define PRINT2(message) printf("message");
#define PRINT3(message) printf(#message);
int main()
{
 PRINT1("if we rest ,we rust\n");
 PRINT2("ATTACK life\n");
 PRINT3("well done\n");
 return 0;
  
}
