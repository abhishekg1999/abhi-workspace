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

ST* searchRec(ST *root, int key){

	if(root == NULL)
		return NULL;

	if(key == root->data) {
		return root;
	} 
	else if (key < root->data) {
		return searchRec(root->left, key);
	}
	else {
		return searchRec(root->right, key);
	}
}

ST* searchItr(ST *root, int key){

	while(root !=NULL){

		if(key == root->data){
			return root;
		}
		else if(key < root->data) {
			root = root->left;
		}
		else
			root = root->right;
	}

	return NULL;
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

	ST *found = searchItr(root, 6);
	if( found!=NULL)
		printf("found:%d\n", found->data);
	else
		printf("Element not found\n");

	return 0;
}
