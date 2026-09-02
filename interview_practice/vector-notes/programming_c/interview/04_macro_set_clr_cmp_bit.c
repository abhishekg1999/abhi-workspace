#include <stdio.h>
#define SETBIT(num, pos) num|=(1<<pos)
#define CLRBIT(num, pos) num&=~(1<<pos)
#define TOGBIT(num, pos) num^=(1<<pos)

void printbits(int num)
{
	for(int pos=31;pos>=0;pos--)
		printf("%d",num>>pos&1);
	printf("\n");
}

int main()
{
	int num=10;
	printf("set macro for set clr and toggle bit \n");
	printbits(num);
	SETBIT(num,2);
	printbits(num);
	CLRBIT(num,2);
	printbits(num);
	TOGBIT(num,2);
	printbits(num);
}
