#include <stdio.h>
void print(char (*p)[5],int ele);
void main()
{
  char s[][5]= {"abcd","efgh","ijkl"};
  print(s,3);
  printf("\n");
}

 void print(char (*p)[5],int ele)
 {
   int i;
   for(i=0;i<ele;i++)
   printf("%s ",p+i);
 }
