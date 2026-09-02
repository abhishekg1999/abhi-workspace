#include <stdio.h>
int main()
{
  int num,pos,op,count=0;
  printf("enter the number\n");
  scanf("%d",&num);
  L1:
  printf("enter the bit position\n");
  scanf("%d",&pos);

  if(pos>=0 && pos<=31)
  { 
  	printf("enter the options..\n");
	printf("1) set-bit 2) clear-bit 3) comp-bit\n ");
	scanf("%d",&op);

	if(op==1)
	{
		num=num|1<<pos;
		printf("bit-set num=%d\n",num);
	}
	else if(op==2)
	{	
		num=num&~(1<<pos);
		printf("bit-clear num=%d\n",num);
	}
	else if(op==3)
	{	
		num=num^1<<pos;
		printf("bit-comp num=%d\n",num);
	}
	else
		printf("unknown option\n");
  }
  else
  {
  	count++;
	printf("enter valid bit postion\n");
	if(count<3)
		goto L1;
	else
		return;
  }
}  
