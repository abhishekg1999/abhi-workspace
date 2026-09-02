/*
Create one thread that prints:

ouput:
0 1 2 3 4 5 6 7 8 9

*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int count =0;
void* thread1(void *p){

	while(1){

		if(count>=10)
			break;

		printf("thread1 =%d\n", count);
		count++;
		sleep(1);
	}

	return 0;
}

int main(){

	pthread_t tid;

	pthread_create(&tid, 0, thread1, 0);
	pthread_join(tid, 0);

	return 0;
}
