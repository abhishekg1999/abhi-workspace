/*
Create:
Producer thread
Consumer thread

Use a shared buffer:
#define SIZE 5

Producer:
0 → 9

Consumer prints:
0 1 2 ... 9

*/

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define size 5
int a[size];   //ring buffer
int head = 0;  //ring buffer head
int tail = 0;  //ring buffer tail

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t send = PTHREAD_COND_INITIALIZER;
pthread_cond_t recv = PTHREAD_COND_INITIALIZER;

void* producer(void *arg){

	for(int i=0; i<10; i++){
		
		pthread_mutex_lock(&lock);
		while( (head+1)%size == tail) {
			
			printf("buffer is full !! waiting ..\n");
			pthread_cond_wait(&send, &lock);   //put producer in sleep, wait for consumer
		} 
			
		a[head] = i;
		printf("producer push : %d\n", i);
		head = (head+1)% size;  //ring buffer logic  head[0-1-2-3-4 0-1--- ]

		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&recv);  //wakeup consumer
		usleep(100000);
	}

	return 0;
}

void* consumer(void *arg) {

	for(int i=0; i<10; i++){
		
		pthread_mutex_lock(&lock);
		while(tail == head) {

			printf("buffer is empty !! waiting..\n");
			pthread_cond_wait(&recv, &lock);  //put consumer in sleep, wait for producer
		}
		
		int data = a[tail];
		printf("consumer pop :%d\n", data);
		tail = (tail+1)% size;   //ring buffer logic tail[0-1-2-3-4 0-1--- ]

		pthread_mutex_unlock(&lock);
		pthread_cond_signal(&send);   //wakeup producer
		usleep(100000);
	}

	return 0;
}

int main(){

	pthread_t tid[2];

	pthread_create(&tid[0], 0, producer, 0);
	pthread_create(&tid[1], 0, consumer, 0);

	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	return 0;
}
