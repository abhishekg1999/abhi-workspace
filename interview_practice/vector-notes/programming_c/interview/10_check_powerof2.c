#include <stdio.h>

int find_powof2(int num){

	if( num>0 && (num&(num-1)) == 0)
		return 1;

	return 0;
}

int main(){

	int num;
	printf("enter the number:\n");
	scanf("%d", &num);

	int ret = find_powof2(num);
	if(ret)
		printf("power of 2\n");
	else
		printf("not power of 2\n");

	return 0;
}
