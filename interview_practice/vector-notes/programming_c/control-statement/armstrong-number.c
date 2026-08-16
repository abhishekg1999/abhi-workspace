#include <stdio.h>

/* it will give the multiplication of given number */
static inline int mul_func(int num, int digit) {

	int temp=1;
	for(int i=0; i<digit; i++)
		temp= temp*num;
	
	return temp;
}

/* it will count len of given number */
static inline int count_digit(int num) {

	int count;
	for (count = 0; num; num=num/10, count++);
	return count;
}

int main()
{
	int num,temp,sum;
	int rem,digit;
	printf("***** Author (abhishek Kumar) *******\n");
	printf("enter the number \n");
	scanf("%d",&num);
	digit=count_digit(num);
	temp = num;

	for(sum=0; temp; temp=temp/10) {
		
		rem=temp%10;
		sum=sum+mul_func(rem,digit);
	}

	if(sum==num) {printf("yes \n");}
	else {printf("not \n");}
}
