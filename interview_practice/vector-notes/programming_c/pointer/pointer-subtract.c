#include <stdio.h>
int main()
{
	int i=1016;
	int j=1000;

	printf("%d\n",i-j);

	int *p=(int*)1016;
	int *q=(int*)1000;

	/* it will give how many integers can be stored between given locations */
	printf("%d\n",p-q);

	/* it will give how many characters can be stored between given locations */
	char *m=(char*)1016;
	char *n=(char*)1000;
	printf("%d\n",m-n);
}
