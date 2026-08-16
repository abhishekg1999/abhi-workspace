#include <stdio.h>
void displaybits(int num)
{
	int pos;
	for(pos=31;pos>=0;pos--)
	{   
        printf("%d",num>>pos&1);
        if(pos%8==0)
            printf(" ");
	}
	printf("\n");
}

int main()
{	
	int num;
		printf("enter the number\n");
		scanf("%d",&num);
		displaybits(num);
}
