#include <string.h>
#include <stdio.h>
int main()
{
 printf("%c\t","Determination"[2]);
 printf("%c\t",*("Determination"+2));
 printf("%s\t","Determination"+2);
 printf("\t");
 printf("Determination"+strlen("Deepali"));
 printf("\t");
 printf("Determination"+sizeof("Deepali"));
 printf("\n");
  
}
