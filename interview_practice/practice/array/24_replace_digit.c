/*
write a program to replace the digit in integer

input:
103201

output:
153251
*/

#include <stdio.h>

int replace_digit(int num, int newdigit){

	int result = 0;
	int placevalue = 1;

	while(num) {

		int digit = num%10;
		if(digit==0)
			digit = newdigit;

		result = result + (digit * placevalue);
		placevalue *=10;
		num = num/10;
	}

	return result;
}

int main(){

	int num = 103201;
	int ret = replace_digit(num, 5);
	printf("%d\n", ret);

	return 0;
}
