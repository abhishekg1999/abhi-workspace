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

/*
8 in binary: 1000 (Exactly one 1 bit)
8 - 1 = 7 in binary: 0111 (The single 1 flipped to 0, and all trailing 0s flipped to 1s) 

trailing 0s : after 3rd bit all 0s are trailing 0s (left to right)
*/
