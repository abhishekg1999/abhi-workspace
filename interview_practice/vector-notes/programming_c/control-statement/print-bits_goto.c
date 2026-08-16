#include <stdio.h>
int main()
{
  int num,pos=31;
  printf("enter the number \n");
  scanf("%d",&num);

  L1:
  printf("%d",num>>pos&1);
  pos--;
  if(pos>=0)
  	goto L1;
  else
  {
  	printf("\n");
  	return 0;
   }
}
