#include <stdio.h>
int main()
{
        int i,j;
        char s[10],ch;

        printf("enter the string \n");
        scanf("%s",s);
        for(i=0;s[i];i++)
        {
              for(j=i+1;s[j];j++)
	      {
                	if(s[i]==s[j])
			{
                        	for(k=i;s[k];k++)
                          	s[k]=s[k+1];

                        	j--; /* for repeated elements */
			}
                }
        }
        printf("string after delete= %s\n",s);
}
