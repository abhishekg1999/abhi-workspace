#include <stdio.h>
int main()
{
	int num,s, num1;
	int count = 0;
	
	for(num=999; count<10; num--)
	{
		num1 = num;
		for(s=0; num1; num1=num1/10)
			s=s*10+num1%10;

		if(s==num)
		{
			count++;
			printf("%d\t",num);
		}
	}
}


