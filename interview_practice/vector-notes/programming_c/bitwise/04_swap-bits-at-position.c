#include <stdio.h>

/* this function is for swaping the bits in number at given position */
int swap_bits(int num,int p,int q)
{
	if((num>>p&1)^(num>>q&1)){ /* compare the bits at p and q position is differ or same if differ then swap otherwise dont swap*/
		num=num^1<<p;
		num=num^1<<q;
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

	int num=31,ret;
	int p=5,q=6;  /* 5nd and 6th bit  in given number*/
	printbits(num);
	ret=swap_bits(num,p,q);
	printbits(ret);
	
}
