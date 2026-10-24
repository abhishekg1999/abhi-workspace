#include <stdio.h>
#define max 5

int a[max],top=-1;
void stack_push(int data);
int stack_pop(void);

int main() {
	
    int choice, val;

    while (1) {
        printf("\n--- Stack Menu ---\n");
        printf("1. Push\n");
        printf("2. Pop\n");
        printf("3. Display Current Stack\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter integer to push: ");
                scanf("%d", &val);
                stack_push(val);
                break;

            case 2:
                val = stack_pop();
                if (val != -1) {
                    printf("Popped value: %d\n", val);
                }
                break;

            case 3:
                if (top == -1) {
                    printf("Stack is empty\n");
                } else {
                    printf("Stack elements (top to bottom): ");
                    for (int i = top; i >= 0; i--) {
                        printf("%d ", a[i]);
                    }
                    printf("\n");
                }
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

	return 0;
}

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
	
	int data =-1;  //in case of underflow return -1
	if(top==-1)
		printf("stack underflow\n");
	else {
		data=a[top];
		top--;
	}

	return data;
}


