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

ST* inOrderPre(ST *root){

	root = root->left;
	while(root->right != NULL)
		root = root->right;

	return root;
}

ST* deleteNode(ST *root, int key){

	ST *iPre;
	if(root ==NULL)
		return NULL;

	if(root->left ==NULL && root->right ==NULL) {
		free(root);
		return NULL;
	}

	/* search the key to be delete */
	if(key < root->data) {
		root->left = deleteNode(root->left, key);
	}
	else if (key > root->data) {
		root->right = deleteNode(root->right, key);
	}

	/* deletion of found node */
	else {
		iPre = inOrderPre(root);
		root->data = iPre->data;
		root->left = deleteNode(root->left, iPre->data);
	}

	return root;
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

	inOrder(root);
	printf("\n");
	deleteNode(root, 3);
	inOrder(root);
        printf("\n");	
	return 0;
}
