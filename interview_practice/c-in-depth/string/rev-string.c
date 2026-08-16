#include <stdio.h>
int main()
{
 int i,j;
 char ch,s[]= "abcdefgh";
 printf("*****befor******\n");
 printf("%s\n",s);
 for(i=0;s[i];i++);
 for(j=0,i=i-1;j<i;j++,i--)
 {
   ch=s[j];
   s[j]=s[i];
   s[i]=ch;
  }

  printf("******after*****\n");
  printf("%s\n",s);
}

