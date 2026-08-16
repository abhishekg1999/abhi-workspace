#include <stdlib.h>
#include <stdio.h>
enum month{jan,feb,mar,apr,may};
int main()
{
 enum month m;
 m=++feb;
 printf("%d\n",m);
 return 0;
  
}
