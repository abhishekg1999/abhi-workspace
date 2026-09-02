#include <stdio.h>
int main()
{
	int num,num1,s;
	printf("enter the number\n");
	scanf("%d",&num);

	num1 = num;
	for(s=0; num1; num1=num1/10)
		s=s*10+(num1%10);
	
	if(s==num)
		printf("yes\n");
	else
		printf("no\n");
}
