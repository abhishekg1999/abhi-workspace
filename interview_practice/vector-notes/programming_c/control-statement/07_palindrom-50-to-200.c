#include <stdio.h>
int main()
{
	int num,num1,s;

 	for(num=50; num<=200; num++)
 	{
		num1 = num;
		for(s=0; num1; num1=num1/10)
			s=s*10+num1%10;
	
		if(s==num)
			printf("%d \n",num);
  	}
}
