#include <stdio.h>
void main()
{
  {
    static int local=10;
	printf("%d\n",local);
	}
	printf("%d\n",local); //error: ‘local’ undeclared (first use in this function)
}
