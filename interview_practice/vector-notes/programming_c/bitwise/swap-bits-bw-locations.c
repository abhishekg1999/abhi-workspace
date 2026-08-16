#include <stdio.h>

/* this function is to swap bits in a number between given locations */
int swap_between(int num,int i,int j)
{
	for(;i<j;i++,j--)
	{
		if((num>>i&1)^(num>>j&1))
		{
			num=num^1<<i;
			num=num^1<<j;
		}
	}
	return num;
}

/* this function is for printing the bits of given number */
void printbits(int num)
{
	int pos;
	for(pos=31;pos>=0;pos--)
		printf("%d",num>>pos&1);
	printf("\n");
}

int main()
{

	int num=3,ret;
	int start=0,end=3;  /* from 2nd to 6th bit of given number*/
	printbits(num);
	ret=swap_between(num,start,end);
	printbits(ret);
	
}
