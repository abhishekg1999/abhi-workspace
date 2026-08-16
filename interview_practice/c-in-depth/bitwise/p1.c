#include <stdlib.h>
#include <stdio.h>
void displaybits(int);
int main()
{
 int x=7,y=19;
 int pos;

 printf("x=>");
 displaybits(x);
 printf("y=>");
 displaybits(y);
	
 printf("%d %d ",x&y,x&&y);
 printf("%d %d ",x|y,x||y);
 printf("\n");
 return 0;
  
}
