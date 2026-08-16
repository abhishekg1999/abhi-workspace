#include <stdlib.h>
#include <stdio.h>
#define CUBE(x) ((x)*(x)*(x))
int main()
{
 int i=1;
 while(i<=8)
 	printf("%d\t",CUBE(i++));

 return 0;
  
}
