/*compare two numbers without using compare operator*/
#include <stdio.h>
int comp_fun(int a,int b)
{
	if(a^b)
		return 1;
	else
		return 0;
}

int main()
{
	int ret;
	ret=comp_fun(10,12);

	if(ret ==1 )
		printf("both are different\n");
	else
		printf("both are same\n");

}
