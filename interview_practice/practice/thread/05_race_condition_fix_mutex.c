#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg)
{
    for (int i = 0; i < 100000; i++){
	pthread_mutex_lock(&lock);
        counter++;
	pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main(){

	pthread_t tid[2];
	pthread_create(&tid[0], 0, thread_func, 0);
	pthread_create(&tid[1], 0, thread_func, 0);

	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	printf("count =%d\n", counter);
	return 0;

}

/*
fix:
If you cannot change the variable type, you must restrict access using a mutex lock so only one thread can touch the counter at a time.
*/
