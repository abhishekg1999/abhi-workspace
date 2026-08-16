#include <stdio.h>
int main()
{
	int n1, n2; //declare two integers
  	printf ("Enter two integer values: ");
  	scanf ("%d%d", &n1, &n2);
 
	(n1^n2) ? printf("both are different\n") : printf("both are same\n"); 
  	return 0;
}
