#include <stdio.h>
int main()
{

  char ch = 'a';
  int m,i,j,k;
  printf("%d %d %d\n",sizeof(char),sizeof(ch),sizeof('a'));
  m=(i=100,j=200,k=300);
  printf("%d %d %d %d\n",m,i,j,k);
  }
