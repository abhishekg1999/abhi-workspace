/*
Find maximum consecutive 1s

input:
int a[] = {1, 1, 0, 1, 1, 1, 0, 1};

output:
3

*/

#include <stdio.h>
int find_max_1s(int a[], int ele){

	int i;
	int temp=0, count=0;

	for(i=0; i<ele; i++){

		if(a[i] == 1)
			temp++;
		else if(temp>count) {
				count=temp;
				temp=0;
		}
		
	}

	return count;
}

int main(){

	int a[] = {1, 1, 0, 1, 1, 1, 1, 0, 1};
	int size = sizeof(a)/sizeof(a[0]);

	int ret = find_max_1s(a, size);
	printf("%d\n", ret);

	return 0;
}
