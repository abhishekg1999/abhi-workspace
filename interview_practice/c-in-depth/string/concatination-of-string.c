#include <stdio.h>
int main()
{ 
  int i,j;
  char a[10]="abcd",b[]="efgh";
  for(i=0;a[i];i++);
  for(j=0;b[j];j++,i++)
    a[i]=b[j];
	a[i]=b[j];

	printf("%s %s\n",a,b);
}


