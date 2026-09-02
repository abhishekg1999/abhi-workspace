#include <stdio.h>
void my_func(char *p)
{
	int i,j,k;
        for(i=0;p[i];i++)
        {
                for(j=i+1;p[j];j++)
                {
                        if(p[i]==p[j])
			{
                                for(k=j;p[k];k++)
                                        p[k]=p[k+1];
			j--; //if continue same element coming 
			}
        
                }
        }
}

int main()
{
	char s[]="hellooo";
	printf("before=%s\n",s);
	my_func(s);
	printf("after=%s\n",s);
	
	return 0;
}
