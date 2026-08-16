#include <stdio.h>
void print(int (*p)[3],int r,int c);
void main()
{
  int b[2][3]={{10,20,30},{40,50,60}};
  int r,c;
  r= sizeof(b)/sizeof(b[0]);
  c= sizeof(b[0])/sizeof(b[0][0]);

  print(b,r,c);
  printf("\n");
}

void print(int (*p)[3],int r,int c)
{
  int i,j;
  for(i=0;i<r;i++)
  for(j=0;j<c;j++)
  printf("%d ",p[i][j]);
}
