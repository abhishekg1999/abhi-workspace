#include<stdio.h>
int main()
{
	int i=10,j=11;
	int * const p=&i;
	p=&j; // error 
}
