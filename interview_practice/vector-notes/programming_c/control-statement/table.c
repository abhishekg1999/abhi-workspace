#include <stdio.h>
int main()
{
  int num,count=1;
  printf("ente the number\n");
  scanf("%d",&num);
  L1:
  printf("%dx%d =%d\n",num,count,num*count);
  count++;
  if(count<=10)
  	goto L1;
  else
  	return ;
}

