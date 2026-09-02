/*
find unique element in array 

input:
int a[]={2,3,5,3,2};

output:
5

*/

#include <stdio.h>
int main(){
	
	
	//int a[]={2,3,5,3,2};
	int a[] = {4, 5, 1, 2, 1, 4, 5};
	int ele = sizeof(a)/sizeof(a[0]);

	int unique=0;
	int temp;
	for(int i=0; i<ele; i++){
	
		unique = unique ^ a[i];
	}
	

	printf("%d\n", unique);
	printf("\n");
	return 0;
}

/*
The Three Magic Rules of XOR:
Rule 1: A number XORed with 0 stays the same. (X ^ 0 = X)
Rule 2: A number XORed with itself becomes 0. (X ^ X = 0)
Rule 3: The order does not matter. (A ^ B ^ C is the exact same as A ^ C ^ B)

How it works:
The loop essentially does this calculation:
2 ^ 3 ^ 5 ^ 3 ^ 2

Let's group the matching pairs together:
(2 ^ 2) ^ (3 ^ 3) ^ 5

Now apply Rule 2 (duplicates cancel out and become 0):
0 ^ 0 ^ 5

Now apply Rule 1 (anything left with 0 stays the same):
0 ^ 5 = 5

*/
