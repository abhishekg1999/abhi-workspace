#include <stdlib.h>
#include <stdio.h>
#define Y 10
int main()
{
 #if X || Y && Z
 	printf("sea in depth\n");
 #else
 	printf("see in depth\n");
 
 #endif
 return 0;
  
}
