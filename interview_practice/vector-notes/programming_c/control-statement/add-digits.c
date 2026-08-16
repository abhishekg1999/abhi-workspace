#include <stdio.h>
int main()
{
	int num,num1,s;
	printf("enter the number\n");

	scanf("%d",&num);
	for(s=0,num1=num;num1;num1=num1/10)
		s=s+(num1%10);
	
	printf("sum = %d\n",s);
}
