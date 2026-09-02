#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1(void *p){

	printf("thread1 trying to lock.. mutex1\n");
	pthread_mutex_lock(&mutex1);
	printf("thread1 locked.. mutex1\n");

	usleep(50000);

	printf("thread1 trying to lock.. mutex2\n");
	pthread_mutex_lock(&mutex2);
	printf("thread1 locked.. mutex2\n");

	pthread_mutex_unlock(&mutex2);
	pthread_mutex_unlock(&mutex1);

	return 0;
}

void* thread2(void *p){

        printf("thread2 trying to lock.. mutex1\n");
        pthread_mutex_lock(&mutex1);
        printf("thread2 locked.. mutex1\n");

        usleep(50000);

        printf("thread2 trying to lock.. mutex2\n");
        pthread_mutex_lock(&mutex2);
        printf("thread2 locked.. mutex2\n");

        pthread_mutex_unlock(&mutex2);
        pthread_mutex_unlock(&mutex1);

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

/*
thread1:
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
pthread_mutex_unlock(&mutex2);
pthread_mutex_unlock(&mutex1);

thread2:
pthread_mutex_lock(&mutex1);
pthread_mutex_lock(&mutex2);
pthread_mutex_unlock(&mutex2);
pthread_mutex_unlock(&mutex1);

explanation:
thread1 --> mutex1  [locked]
thread2 --> mutex1  [waiting.. to lock mutex1, because thread1 holding mutex1]
thread1 --> mutex2  [locked]
now thread1 will finish the work and release both mutex1 and mutex2.
thread2 --> mutex1  [locked]
thread1 --> mutex1  [waiting.. to lock mutex1, because thread2 holding mutex1]
thread2 --> mutex2  [locked]
now thread2 finish the work and release mutex1 and mutex2
same continue ... deadlock fixed here 

output:
thread2 trying to lock.. mutex1
thread2 locked.. mutex1
thread1 trying to lock.. mutex1
thread2 trying to lock.. mutex2
thread2 locked.. mutex2
thread1 locked.. mutex1
thread1 trying to lock.. mutex2
thread1 locked.. mutex2

*/
