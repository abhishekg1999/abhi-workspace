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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define size 5

int buffer[size];
int count =0;
int in =0;
int out =0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

void* producer(void *p){

	for(int i=0; i<=9; i++){

		pthread_mutex_lock(&lock);   //lock thread1
		while(count == size)
			pthread_cond_wait(&cond1, &lock);  //wait if buffer full, wait for thread2

		buffer[in] = i;
		in = (in+1)%size;
		count++;

		pthread_cond_signal(&cond2);   //wake up thread2
		pthread_mutex_unlock(&lock);   //unlock thread1
	}

	return 0;
}


void* consumer(void *p){

        for(int i=0; i<=9; i++){
		
		pthread_mutex_lock(&lock);  //lock thread2
                while(count == 0)
                        pthread_cond_wait(&cond2, &lock); //wait if buffer empty, wwait for thread1

                int item = buffer[out];
                out = (out+1)%size;
                count--;
		
		printf("item = %d\n", item);

                pthread_cond_signal(&cond1);   //wake up thread1
                pthread_mutex_unlock(&lock);   //unlock thread2
        }

        return 0;
}

int main(){

	pthread_t tid[2];
	pthread_create(&tid[0], 0, consumer, 0);
	pthread_create(&tid[1], 0, producer, 0);

	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);

	return 0;
}
