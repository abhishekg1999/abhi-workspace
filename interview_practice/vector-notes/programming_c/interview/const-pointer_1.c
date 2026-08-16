/*define a const pointer which can change the value but not address*/
#include <stdio.h>

int main()
{
	int x,y;
	int *const p=&x;

	*p=10;
	p=&y;
}
