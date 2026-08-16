#include <stdio.h>
#include <stdlib.h>

typedef struct student
{
	int rollno;
	char name[10];
	float marks;
	struct student *next;
}ST;

void read_file(ST **);
void print_list(ST *p);

int main()
{
	char ch;
	ST *hp=0;

	read_file(&hp);  //creating link list
	print_list(hp);       //printing link list
}

void read_file(ST **ptr){

        ST buffer, *new, *temp;
        FILE *fp = fopen("file", "r");

        while(fscanf(fp, "%d %s %f ", &buffer.roll, buffer.name, &buffer.marks) != -1) {

                new = malloc(sizeof(ST));  //allocate new node
                *new = buffer;             //copy data to new node

                if(*ptr == 0){

                        new->next =0;    //make 0 to end of new node
                        *ptr = new;      //update head pointer with new node
                        temp = new;      //temp hold last node 
                }
                else {
                        new->next = 0;       //make 0 to end of new node
                        temp->next = new;   //link end of temp with new node
                        temp = new;         //temp hold last node 
                }
        }
}

void print_list(ST *p){

        while(p){

                printf("roll =%d name =%s marks =%f\n", p->roll, p->name, p->marks);
                p = p->next;
        }
}
