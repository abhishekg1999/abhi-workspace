/*
	   4
         /  \
       1     6
      / \  
     5  2  
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

int main(){

	/* create nodes */
	ST *root = createNode(4);
	ST *n2 = createNode(1);
	ST *n3 = createNode(6);
	ST *n4 = createNode(5);
	ST *n5 = createNode(2);

	/* link nodes */
	root->left =n2;
	root->right =n3;

	n2->left =n4;
        n2->right =n5;

	/* print the root node data */
	printf("root data =%d\n", root->data);
	return 0;
}
