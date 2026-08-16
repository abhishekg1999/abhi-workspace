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
ST* search_node(ST *,int);
int main()
{
	char ch;
	ST *hp=0;
	int rollno;

	read_file(&hp);  //creating link list
	print_rec(hp);       //printing link list
	
	printf("enter the rollno \n");
	scanf("%d",&rollno);
	ST *ret = search_node(hp, rollno);
        if( ret == 0)
                printf("node is not present\n");
        else
                printf("node adds =%p\n", ret);
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

ST* search_node(ST *ptr,int rollno)
{
	while(ptr)
	{
		if(ptr->rollno==rollno)
		return ptr;

	    ptr=ptr->next;
	}
	return 0;
}

void print_rec(ST *p) 
{
    if(p)
    {
        printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
        print_rec(p->next);
    }
}

