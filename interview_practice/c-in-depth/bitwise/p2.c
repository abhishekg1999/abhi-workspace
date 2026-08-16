#include <stdlib.h>
#include <stdio.h>
void displaybits(int);
int main()
{
 unsigned int x,y,z;
 displaybits(0xFFFF);
 x=y=z=0xFFFF;
 x=(x>>5)<<5;  displaybits(x);
 y=(y>>3)<<3;  displaybits(y);
 z=(z>>2)<<2;  displaybits(z);
 return 0;
  
}
