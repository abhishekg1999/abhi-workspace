#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_int counter = 0;
void *thread_func(void *arg) {

	for (int i = 0; i < 100000; i++)
		counter++;

	return NULL;
}

int main() {

	pthread_t tid[2];
	pthread_create(&tid[0], 0, thread_func, 0);
	pthread_create(&tid[1], 0, thread_func, 0);

	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	printf("count =%d\n", counter);
	return 0;
}

