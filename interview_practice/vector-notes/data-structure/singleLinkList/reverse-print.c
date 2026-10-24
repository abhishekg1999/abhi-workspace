#include <stdio.h>
#include <stdlib.h>
typedef struct student
{
	int rollno;
	char name[10];
	float marks;
	struct student *next;
}ST;

void print_rec(ST*);
void print_rec_rev(ST*);
void add_begin(ST**);

int main()
{
	char ch;
	ST *hp=0;
	do
	{
		add_begin(&hp);
		printf("want to add one more node (y/n):");
		scanf(" %c",&ch);
	}while(ch=='y');
	print_rec(hp);
	printf("after rev\n");
	print_rec_rev(hp);
}

void add_begin(ST **ptr)
{
	ST *new;
	new=malloc(sizeof(ST));
	printf("enter the roll,name,marks\n");
	scanf("%d%s%f",&new->rollno,new->name,&new->marks);

	new->next=*ptr;
	*ptr=new;
}

void print_rec(ST *p)
{
    if(p)
    {
        printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
        print_rec(p->next);
    }
}

void print_rec_rev(ST *p)
{
    if(p)
    {
        print_rec_rev(p->next);
        printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
    }
}
