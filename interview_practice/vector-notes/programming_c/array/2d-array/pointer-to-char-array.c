#include <stdio.h>

#define r 3
#define c 10

void print(char (*p)[10],int row)
{
        for(int i=0;i<row;i++)
                printf("%s\n",p[i]);
}

int main()
{
	char s[3][10]={"abcd","efgh","ijkl"};
	print(s, 3);
}
