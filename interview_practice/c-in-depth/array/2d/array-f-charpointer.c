#include <stdio.h>
void main()
{
  char *p[]= {"abcd","efgh","ijkl"};
  printf("sizeof p= %d\n",sizeof(p));
  printf("sizeof p[0]= %d\n",sizeof(p[0]));
  printf("sizeof p[0][0]= %d\n",sizeof(p[0][0]));
  //p[0][0]++; //cant modify text section
  printf("%s\n",p[0]);
  printf("%c\n",*(p[0]+2));

 }
