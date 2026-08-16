#include <stdio.h>
void main()
{
  int sample[2][3];
  int r,c;
  r= sizeof(sample)/sizeof(sample[0]);               //no of elements in 2-d array
  c= sizeof(sample[0])/sizeof(sample[0][0]);         //no of sub-elements in one element/1-d array
  printf("size of sample= %ld\n",sizeof(sample));
  printf("r= %d c= %d\n",r,c);
 }
