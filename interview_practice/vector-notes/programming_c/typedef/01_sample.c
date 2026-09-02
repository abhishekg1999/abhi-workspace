#include <stdio.h>
typedef int INT;
typedef int A[5];
typedef int *IPTR;

typedef struct student
{
	int roll;
	char name[10];
	float marks;
}stu;

int main()
{
	int i;
	INT j;
	A a;
	printf("sizeof(a)=%d\n",sizeof(a));
	printf("sizeof(i)=%d\tsizeof(j)=%d\n",sizeof(i),sizeof(j));
}
