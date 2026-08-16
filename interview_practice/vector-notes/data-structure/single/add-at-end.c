#include <stdio.h>
#include <stdlib.h>

typedef struct student {

	int roll;
	char name[10];
	float marks;
	struct student *next; //self reference pointer 
} ST;

void print_list(ST *p);
void add_end(ST **ptr);

int main() {

	char ch;
	ST *hp=0;
	do{
		add_end(&hp);
		printf("want to add one more node (y/n):");
		scanf(" %c", &ch);
	}while(ch == 'y');
	
	printf("##### link list ######\n");
	print_list(hp);
}

void add_end(ST **ptr){

	ST *new, *temp;
	new = malloc(sizeof(ST));
	printf("enter the roll, name, marks\n");
	scanf("%d%s%f", &new->roll, new->name, &new->marks);

	if(*ptr ==0){
		new->next=0;
		*ptr = new;
	} 
	else {
		temp = *ptr;   //here, temp hold first node address or head pointer
		while(temp->next) 
			temp = temp->next;    //here, temp hold last node address

		new ->next =0;
		temp ->next = new;  
	}
}

void print_list(ST *p){

	while(p){

		printf("roll =%d name =%s marks =%f\n", p->roll, p->name, p->marks);
		p = p->next;
	}
}
