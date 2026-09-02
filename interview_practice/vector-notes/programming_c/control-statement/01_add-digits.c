#include <stdio.h>
int main()
{
	int num, s;
	printf("enter the number\n");

	scanf("%d",&num);
	for(s=0; num; num=num/10)
		s=s+(num%10);
	
	printf("sum = %d\n",s);
}
