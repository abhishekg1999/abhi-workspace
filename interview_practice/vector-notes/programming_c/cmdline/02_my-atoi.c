#include <stdio.h>

int my_atoi(const char *p){
	
	int num=0, i;
	
	if(p[0] == '-' || p[0] == '+')
		i=1;
	else
		i=0;
	for(; p[i]; i++){
		if(p[i]>='0' && p[i]<='9')
			num = num*10 + (p[i]-48);
		else
			break;
	}
	
	if(p[0] =='-')
		num = -num;
		
	return num;
}
			
int main(int argc, char *argv[]){
	
	int i;
	if(argc<2){
		printf("need to 2 arguments:\n");
		return -1;
	}
	
	i=my_atoi(argv[1]);
	printf("len =%d\n", i);
	return 0;
}
