#include <stdio.h>
#include <string.h>
void main()
{
  char temp[5],s[5][5];
  int i,j,ele;
  ele=sizeof(s)/sizeof(s[0]);
  for(i=0;i<ele;i++)
  scanf("%s",s[i]);

  for(i=0;i<ele;i++)
  printf("%s ",s[i]);
  printf("\n");

  for(i=0;i<ele-1;i++)
  for(j=0;j<ele-1-i;j++)
  {
    if(strcmp(s[j],s[j+1])>0)
	{
	  strcpy(temp,s[j]);
	  strcpy(s[j],s[j+1]);
	  strcpy(s[j+1],temp);
	  }

	}

	for(i=0;i<ele;i++)
	printf("%s ",s[i]);
	printf("\n");
}
