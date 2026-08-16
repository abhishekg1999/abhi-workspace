#include <stdio.h>
extern int i;
void main()
{
   extern int j;
   extern int k;
   extern int l;
  //extern int i;
  printf("%d\n",i);
 // printf("static j= %d\n",j); //does not have external linkage propertie(undefined reference to `j')
  //printf("abc k = %d\n",k); //auto variable have scope within abc only(undefined reference to `k')
  //printf("abc static l= %d\n",l); //static local variable have scope is within abc only (undefined reference to `l')
 }
