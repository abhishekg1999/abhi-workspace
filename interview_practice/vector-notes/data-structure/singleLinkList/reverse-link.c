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
void rev_link(ST **);
int main()
{
	char ch;
	ST *hp=0;

	read_file(&hp);  //creating link list
	print_rec(hp);       //printing link list
	rev_link(&hp);
	printf("******after reverse link********\n");
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

void rev_link(ST **ptr){

        ST *p, *q, *r;

        p=*ptr;
        q=0;
        while(p) {

                r=q;  //r hold previos node 
                q=p;  //q hold current node
                p = p->next;  //p moved to next node
                q->next = r;   //link current node end to privous node 
        }

        *ptr = q;  //updating rev link to hp
}

void print_rec(ST *p) 
{
    if(p)
    {
        printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
        print_rec(p->next);
    }
}

