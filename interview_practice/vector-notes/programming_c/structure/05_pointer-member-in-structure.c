#include <stdio.h>
#include <stdlib.h>
typedef struct {
	int rollno;
	char *name;
	//char name[10]; statically at compile time memory allocated
	float marks;
}student;

int main()
{
	student *p;
	p=malloc(sizeof(student)); //run time memory allocated
	p->name=malloc(10);

	printf("enter the data\n");
	scanf("%d%s%f",&p->rollno,p->name,&p->marks);
	printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
}
