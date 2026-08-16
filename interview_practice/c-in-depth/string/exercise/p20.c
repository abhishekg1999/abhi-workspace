#include <string.h>
#include <stdio.h>
int main()
{
 char *str[10];
 int i;
 for(i=0;i<10;i++)
 	scanf("%s",str[i]); //error memory not allocated for str[i]

 for(i=0;i<10;i++)
 	scanf("%s",str[i]);
 return 0;
  
}
