#include <stdio.h>
int my_atoi(const char *);

int main()
{
	char s[]="1234";
	int i;

	i=my_atoi(s);
	printf("string=%s\thex=%x\n",s,i);
}

int my_atoi(const char *p)
{
	int i,num=0;

	if(p[0]=='-' || p[0] == '+')
		i=1;
	else
		i=0;

	for(;p[i];i++){
		if(p[i]>='0' && p[i]<='9')
			num=num*10+p[i]-48;
		else
			break;
	}

	if(p[0]=='-')
		num=-num;
		
	return num;
}
