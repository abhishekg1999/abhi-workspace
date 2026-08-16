#include <stdio.h>
int main()
{
	int num,s,count,num1;
	
	for(num=999,count=0;count<10;num--)
	{
		for(num1=num,s=0;num1;num1=num1/10)
			s=s*10+num1%10;

		if(s==num)
		{
			count++;
			printf("%d\t",num);
		}
	}
}


