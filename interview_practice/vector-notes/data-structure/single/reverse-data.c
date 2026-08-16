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
void rev_data(ST *);

int main()
{
	char ch;
	ST *hp=0;

	read_file(&hp);  //creating link list
	print_rec(hp);       //printing link list
	rev_data(hp);
	printf("******after reverse********\n");
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

void rev_data(ST *ptr)
{
	ST **p,*temp,buffer;
	int i, j, count=3;

	p=malloc(sizeof(ST*)*count);  //assume link list has 3 nodes

	i=0;
	temp=ptr;  //hp to temp
	//saving data in array of pointer
	while(temp) {
		
		p[i++]=temp;
		temp=temp->next;
	}

	//swaping of nodes 
	for(i=0, j=count-1; i<j; i++, j--) {
		
		buffer.rollno=p[i]->rollno;
		strcpy(buffer.name,p[i]->name);
		buffer.marks=p[i]->marks;

		p[i]->rollno=p[j]->rollno;
                strcpy(p[i]->name,p[j]->name);
		p[i]->marks=p[j]->marks;

		p[j]->rollno=buffer.rollno;
                strcpy(p[j]->name,buffer.name);
		p[j]->marks=buffer.marks;
	}

}

void print_rec(ST *p) 
{
    if(p)
    {
        printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
        print_rec(p->next);
    }
}

