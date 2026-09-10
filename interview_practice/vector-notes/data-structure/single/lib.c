// this is node
typedef struct student {

	int roll;
	char name[10];
	float marks;
	struct student *next; //self reference pointer 
} ST;

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

/* prototypes */
void print_list(ST *p);
void add_end(ST **ptr);
void add_begin(ST **ptr);
void add_middle(ST **ptr);
int count_nodes(ST *p);
void save_file(ST *p);
void read_file(ST **ptr);
void delete_node(ST **ptr);
void rev_data(ST *ptr);
ST* search_node(ST *ptr, int roll);
void rev_link(ST **ptr);
void print_rec(ST *p);
void print_rec_rev(ST *p);
void stack_push(int data);
int stack_pop(void);

void add_begin(ST **ptr) {

	ST *new;
	new = malloc(sizeof(ST));
	printf("enter the roll, name, marks:\n");
	scanf("%d%s%f", &new->roll, new->name, &new->marks);

	new->next = *ptr;  //link head pointer to end of new node
	*ptr = new;        //update head pointer with new node
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
		temp = *ptr;   //here, temp hold first node or head pointer
		while(temp->next) 
			temp = temp->next;    //here, temp hold last node in link list

		new ->next =0;
		temp ->next = new;  
	}
}

void add_middle(ST **ptr){

	ST *new, *temp;
	new = malloc(sizeof(ST));

	printf("enter the roll, name, marks:\n");
        scanf("%d%s%f", &new->roll, new->name, &new->marks);
	
	/* in case of zero node or one node in list */
	if(*ptr == 0 || (*ptr)->roll > new->roll){  

		new->next = *ptr; //link head pointer to end of new node
		*ptr = new;	  //update head pointer with new node
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

void delete_node(ST **ptr){

	int roll;
	ST *temp;   //hold current node 
	ST *temp1;  //hold previous node

	printf("enter the roll of node want to delete:\n ");
	scanf("%d", &roll);
	temp = *ptr;

	while(temp){

		if(temp->roll == roll){   //if roll matched with roll of node

			if(temp == *ptr)  //if it is first node
				*ptr = temp->next;
			else
				temp1->next = temp->next;   //if it is other node

			free(temp);
			return;
		}

		temp1 = temp;
		temp = temp->next;
	}

	printf("node is not present\n");
}

ST* search_node(ST *ptr, int roll){

	while(ptr){

		if(ptr->roll == roll)
			return ptr;

		ptr = ptr->next;
	}

	return 0;
}

int count_nodes(ST *p){

	int count =0;
	while(p){

		count ++;
		p = p->next;
	}
	
	return count;
}

void print_list(ST *p){

	while(p){

		printf("roll =%d name =%s marks =%f\n", p->roll, p->name, p->marks);
		p = p->next;
	}
}

void print_rec(ST *p)
{
	if(p){
		
		printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
		print_rec(p->next);
	}
}

void print_rec_rev(ST *p)
{
    if(p){
        
        print_rec_rev(p->next);
        printf("rollno=%d\tname=%s\tmarks=%f\n",p->rollno,p->name,p->marks);
    }
}

void rev_data(ST *ptr) {
	
	ST **p;   //hold array of pointer
	ST *temp,  //for traverse the list
	ST buffer;  //swap buffer
	 
	int i, j, count=3;  //lets take 3 nodes or you can use "count_node()"

	p = malloc(sizeof(ST*)*count);  //create array of pointer, 
	temp = ptr;

	/* fill array of pointer with nodes */
	i=0;
	while(temp){

		p[i++] = temp;
		temp = temp->next;
	}

	/* swap the nodes */
	for(i=0, j=count-1; i<j; i++, j--) {
		
		buffer.roll = p[i]->roll;
		strcpy(buffer.name, p[i]->name);
		buffer.marks = p[i]->marks;

		p[i]->roll = p[j]->roll;
		strcpy(p[i]->name, p[j]->name);
		p[i]->marks = p[j]->marks;

		p[j]->roll = buffer.roll;
		strcpy(p[j]->name, buffer.name);
		p[j]->marks = buffer.marks;
	}
}

void rev_link(ST **ptr){

	ST *p;  //traverse the list
	ST *q;  //hold current node
	ST *r;  //hold privous node

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

void save_file(ST *p){

	FILE *fp;
	fp = fopen("file", "w");

	while(p){

		fprintf(fp, "%d %s %f ", p->roll, p->name, p->marks);
		p = p->next;
	}
}

void read_file(ST **ptr){

	ST buffer;  //scan data from file into buffer
	ST *new;    //new node 
	ST *temp;   //hold previous node
	
	FILE *fp = fopen("file", "r");
	while(fscanf(fp, "%d %s %f ", &buffer.roll, buffer.name, &buffer.marks) != -1) {

		/* create new node and copy buffer data into new node */
		new = malloc(sizeof(ST)); 
		*new = buffer;

		/* make link list add_at_end*/
		if(*ptr == 0){

			new->next =0;    //make null to end of new node
			*ptr = new;      //update head pointer with new node
			temp = new;      //temp hold last node 
		}
		else {
			new->next = 0;       //make null to end of new node
			temp->next = new;   //link end of temp with new node
			temp = new;         //temp hold last node 
		}
	}
}

#define max 5
int a[max],top=-1;

//LIFO
void stack_push(int data) {
	
	top++;
	if(top==max){
		
		printf("stack overflow\n");
		top--;
	}
	else
		a[top]=data;
}

int stack_pop(void) {
	
	int data;
	if(top==-1){
		printf("stack underflow\n");
		return -1;
	}
	else {
		data=a[top];
		top--;
	}

	return data;
}
