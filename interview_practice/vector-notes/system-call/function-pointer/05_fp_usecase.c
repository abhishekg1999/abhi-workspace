#include <stdio.h>

void my_open(void) {

	printf("my_open called...\n");
}

void my_close(void) {

        printf("my_close called...\n");
}

int my_add(int a, int b){

	printf("my_add called ..\n");
}

int my_sub(int a, int b){

	printf("my_sub called ..\n");
}

struct my_operations {

	void (*open)(void);
	int (*add)(int, int);
	int (*sub)(int, int);
	void (*close)(void);
};


int main(){

	struct my_operations myops = {

		.open = my_open,
		.add = my_add,
		.sub = my_sub,
		.close = my_close,
	};

	int ret;
	myops.open();
	ret = myops.add(10, 20);
	ret = myops.sub(10, 20);
	myops.close();

	return 0;
}
