#include <stdio.h>
#include <stdlib.h>
typedef struct student
{
	int rollno;
	char name[10];
	float marks;
	struct student *next;
}ST;

void add_middle(ST**);
void save_file(ST *);

int main()
{
	char ch;
	//creating link list
	ST *hp=0;
	do
	{
		add_middle(&hp);
		printf("want to add one more node (y/n):");
		scanf(" %c",&ch);
	}while(ch=='y');

	//saving data into file
	printf("writing the data into file...\n");
	save_file(hp);
}

void add_middle(ST **ptr)
{
	ST *new,*temp;
	new=malloc(sizeof(ST));
	printf("enter the roll,name,marks\n");
	scanf("%d%s%f",&new->rollno,new->name,&new->marks);

	if(*ptr==0||(*ptr)->rollno > new->rollno)
	{
	new->next=*ptr;  //assign head pointer to new node next pointer
	*ptr=new;        //update hp with new node
	}
	else
	{
		temp=*ptr;
		while(temp)
		{
			if(temp->next==0 || temp->next->rollno > new->rollno)
			{
				new->next=temp->next;  //update new->next either with 0 or node adds
				temp->next=new;        //update temp->next with new node adds
				break;
			}
			temp=temp->next;
	     }
     }
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
