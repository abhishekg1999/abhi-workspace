#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student
{
	int rollno;
	char name[10];
	float marks;
	struct student *next;
}ST;

void read_file(ST **);
void print_rec(ST *);
void delete_node(ST **);

int main()
{
	char ch;
	ST *hp=0;

	read_file(&hp);  //creating link list
	print_rec(hp);       //printing link list
	delete_node(&hp);
	printf("******after delete node********\n");
	print_rec(hp);
}

void read_file(ST **ptr)
{
	ST buffer,*new,*temp;
	FILE *fp=fopen("file","r");

	while(fscanf(fp,"%d %s %f ",&buffer.rollno,buffer.name,&buffer.marks)!=-1)
	{
		new=malloc(sizeof(ST));
		*new=buffer;

		if(*ptr==0)
		{
			*ptr=new;
			new->next=0;
			temp=new;
		}
		else
		{	
			temp->next=new;
			new->next=0;
			temp=new;
		}
	}
}

void delete_node(ST **ptr)
{
	int rollno;
	ST *temp,*temp1;

	printf("enter the rollno of node you want to delete\n");
	scanf("%d",&rollno);
	temp=*ptr;

	while(temp)
	{
		if(temp->rollno==rollno)
		{
			if(temp==*ptr)
				*ptr=temp->next;
			else
				temp1->next=temp->next;

			free(temp); //free memory of the deleted node 
			return ;
		}

		temp1=temp;         //hold previous node.
		temp=temp->next;   //hold current node
	}
	printf("node is not present\n");
}

void print_rec(ST *p) 
{
    if(p)
    {
        printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
        print_rec(p->next);
    }
}

