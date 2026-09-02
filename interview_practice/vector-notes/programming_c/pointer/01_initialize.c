#include <stdio.h>
int main()
{
	int i=10;
	int *p;
	*p=&i;
    	printf("*p=%u p=%u i=%u\n",*p,p,&i);
    
	//int *p=i;
	//printf("%u %u\n",p,*p);

	//int *p=&i;
	//printf("%u %u \n",p,&i);
}
