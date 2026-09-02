#include <stdio.h>
int main()
{

	int i,count=0,a[10]={1,2,3,4,5,121,131,232,242,414};
	int num,s;

	for(i=0; i<10; i++) {	
		
		num=a[i];
		for(s=0; num; num=num/10)
			s=s*10+num%10;

		if(s==a[i])
			count++;
	}

	printf("count=%d\n",count);
}
