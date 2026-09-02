#include <stdio.h>
int main()
{
	int i=10;
	int const *p=&i;
	/* or */
	//const int *p=&i;
	*p=20; // error 
}
