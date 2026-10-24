/*
	   5
         /  \
       3     6
      / \  
     1  4  
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct node {

	int data;
	struct node *left;
	struct node *right;
}ST;

ST* createNode(int data) {

	ST *node = malloc(sizeof(ST));
	node->data =data;
        node->left =NULL;
        node->right =NULL;

	return node;
}

void insert(ST *root, int key){

	ST *prev = NULL;
	while(root !=NULL){

		prev = root;
		if(key == root->data) {
			printf("already exist, can not insert\n");
			return ;
		}
		else if(key < root->data){
			root = root->left;
		}
		else {
			root = root->right;
		}
	}

	/* make link */
	ST *new = createNode(key);
	if(key <prev->data)
		prev->left = new;
	else
		prev->right = new;
}

int main(){

	/* create nodes */
	ST *root = createNode(5);
	ST *n1 = createNode(3);
	ST *n2 = createNode(6);
	ST *n3 = createNode(1);
	ST *n4 = createNode(4);

	/* link nodes */
	root->left =n1;
	root->right =n2;

	n1->left =n3;
        n1->right =n4;
	
	insert(root, 7);
	printf("%d\n", n2->right->data);
	return 0;
}
