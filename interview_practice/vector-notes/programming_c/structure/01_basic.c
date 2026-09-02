#include <stdio.h>
typedef struct {
 	char ch;
	int i;
	float f;
}temp;

temp s = {
	.ch ='a',
	.i=10,
	.f=10.5
};

int main()
{
	// temp x={'a',40,40.2};       

       /*    or         */
	
	/*	
	 temp buf = {
                'a',
                10,
                10.5
        };
	*/

	/*        or      */

	/*
	temp x={
		.ch='a',
		.i=40,
		.f=40.2
	};
	*/

	printf("%c\t%d\t%f\n", s.ch, s.i, s.f);
	printf("sizeof x=%ld\n",sizeof(s));
}
