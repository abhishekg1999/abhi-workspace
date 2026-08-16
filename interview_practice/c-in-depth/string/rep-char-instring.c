#include <stdio.h>
int main()
{
  int i;
  char s[]="embedded",ch1,ch2;
  printf("enter the characters\n");
  scanf("%c %c",&ch1,&ch2);
  printf("befor==>%s\n",s);
  for(i=0;s[i];i++)
  {
    if(s[i]==ch1)
	s[i]=ch2;
	}

	printf("after==>%s\n",s);
}

