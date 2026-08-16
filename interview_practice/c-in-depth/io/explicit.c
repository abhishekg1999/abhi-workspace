#include <stdio.h>
int main()
{
 unsigned long int i=20000000000, j=3;
 unsigned long long int k;
 k=(unsigned long long int)i*j;

// k=i*j;
 printf("k=%llu\n",k);
 }
