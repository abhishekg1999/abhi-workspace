#include <stdio.h>
struct student
{
	int rollno;
	char name[10];
	float marks;
};

int main()
{
	struct student stu;

	printf("enter the rollno name marks\n");
	scanf("%d%s%f",&stu.rollno,stu.name,&stu.marks);

	printf("student data is below :\n");
	printf("rollno=%d\tname=%s\tmarks=%f\n",stu.rollno,stu.name,stu.marks);
}
