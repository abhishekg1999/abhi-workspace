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

void inOrder(ST *root){

	if(root ==NULL)
		return;

	inOrder(root->left);
	printf("%d ", root->data);
	inOrder(root->right);
}

int is_BST(ST *root){

	static ST *prev = NULL;
	if(root == NULL)
		return 1;

	if(!is_BST(root->left))
		return 0;
	if(prev !=NULL && root->data <= prev->data)
		return 0;

	prev = root;
	return is_BST(root->right);
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

	/* print BT in pre order */
	inOrder(root);
	printf("\n");
	printf("%d\n", is_BST(root));
	return 0;
}
