#include <stdio.h>
#include <stdlib.h>
typedef struct student
{
	int rollno;
	char name[10];
	float marks;
	struct student *next;
}ST;

void print(ST*);
void add_middle(ST**);
int count_nodes(ST *);
void save_file(ST *);
int main()
{
	char ch;
	ST *hp=0;
	do
	{
		add_middle(&hp);
		printf("want to add one more node (y/n):");
		scanf(" %c",&ch);
	}while(ch=='y');
	print(hp);
	printf("no of nodes= %d\n",count_nodes(hp));
	printf("writing the data into file...\n");
	save_file(hp);
}

void add_middle(ST **ptr){

        ST *new, *temp;
        new = malloc(sizeof(ST));

        printf("enter the roll, name, marks:\n");
        scanf("%d%s%f", &new->roll, new->name, &new->marks);

        /* in case of zero node or one node in list */
        if(*ptr == 0 || (*ptr)->roll > new->roll){

                new->next = *ptr; //link head pointer to end of new node
                *ptr = new;       //update head pointer with new node
        }
        else {

                temp = *ptr;
                while(temp) {

                        if(temp->next ==0 || temp->next->roll > new->roll){

                                new->next = temp->next; //link end of temp to end of new node
                                temp->next = new;       //link end of temp with new node
                                break;
                        }

                        temp = temp->next;
                }
        }
}

void print(ST *p)
{
	while(p)
	{
		printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
		p=p->next;
	}
}

int count_nodes(ST *p)
{
	int count=0;
	while(p)
	{
		count++;
		p=p->next;
	}
	return count;
}

void save_file(ST *p)
{
	FILE *fp;
	fp=fopen("file","w");

	while(p)
	{
		fprintf(fp,"%d %s %f ",p->rollno,p->name,p->marks);
		p=p->next;
	}
}
