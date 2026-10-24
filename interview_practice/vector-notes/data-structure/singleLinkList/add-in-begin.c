#include <stdio.h>
#include <stdlib.h>
typedef struct student
{
	int rollno;
	char name[10];
	float marks;
	struct student *next;
}ST;

void print_list(ST*);
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
	
	printf("##### link list ######\n");
        print_list(hp);

}

void add_begin(ST **ptr) {

        ST *new;
        new = malloc(sizeof(ST));
        printf("enter the roll, name, marks:\n");
        scanf("%d%s%f", &new->roll, new->name, &new->marks);

        new->next = *ptr;  //link head pointer to end of new node
        *ptr = new;        //update head pointer with new node
}


void print_list(ST *p){

        while(p){

                printf("roll =%d name =%s marks =%f\n", p->roll, p->name, p->marks);
                p = p->next;
        }
}

