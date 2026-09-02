/*
Both threads increment:
int counter = 0;

Each thread performs:
count++;

output:
thread1 print 1 to 10
thread2 print 11 to 20

*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int count =0;
void* thread1(void *p){

	for(int i=0; i<10; i++){
		count++;
		printf("thread1 =%d\n", count);
	}

	return 0;
}

void* thread2(void *p){

        for(int i=0; i<10; i++){
                count++;
		printf("thread2 =%d\n", count);
	}

        return 0;
}

int main(){

	pthread_t tid[2];

	pthread_create(&tid[0], 0, thread1, 0);
	pthread_create(&tid[1], 0, thread2, 0);
	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	return 0;
}
