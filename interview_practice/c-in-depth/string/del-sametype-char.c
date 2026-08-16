#include <stdio.h>
int main()
{
  int i,j;
  char s[]="abcdaabbaefg",ch='a';
  printf("before==>%s\n",s);
  for(i=0;s[i];i++)
  {
    if(s[i]==ch)
	{
	  for(j=i;s[j];j++)
	  s[j]=s[j+1];
	  i--;
	 }
   }
   printf("after==>%s\n",s);
 }
