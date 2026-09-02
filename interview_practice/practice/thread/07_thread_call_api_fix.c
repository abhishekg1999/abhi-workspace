#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int global_status = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int get_status(void) {
    
    int current_status;
    pthread_mutex_lock(&lock);
    current_status = global_status;
    pthread_mutex_unlock(&lock);

    return current_status;
}

void* status_updater(void* arg) {
    
    for (int i = 1; i <= 3; i++) {
        sleep(1); // Simulate work

	pthread_mutex_lock(&lock);
        global_status = i; // Safely update status
	pthread_mutex_unlock(&lock);
	printf("Writer data: %d\n", global_status);
    }
    return NULL;
}

void* status_reader(void* arg) {
    
    for (int i = 0; i < 5; i++) {
        int current = get_status(); // Calling the thread-safe function
        printf("Read data: %d\n", current);
        usleep(500000); // Check every 500ms
    }

    return NULL;
}

int main() {
    
	pthread_t tid[2];
    	pthread_create(&tid[0], NULL, status_updater, NULL);
	pthread_create(&tid[1], NULL, status_reader, NULL);

    	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
    	return 0;
}
