/*

task:
take count =0,  thread1 should print even, thread 2 should print odd 

consider:
count only 0 to 10

note : but here its not possible so go for condition variables 
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int count=0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* thread1(void *p){

	while(1){
		
		pthread_mutex_lock(&lock);
		if(count%2==0)
			printf("in thread1 = %d\n", count);

		if(count >=10){
                        pthread_mutex_unlock(&lock);
                        break;
                }

		count++;
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
}

void* thread2(void *p){

	while(1) {

		pthread_mutex_lock(&lock);
		if(count%2 !=0)
        		printf("in thread2 =%d\n", count);

		if(count >=10){
                        pthread_mutex_unlock(&lock);
                        break;
                }

		count++;
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
}

int main(){

        pthread_t tid[2];  //thread id

        pthread_create(&tid[0], 0, thread1, 0);
        pthread_create(&tid[1], 0, thread2, 0);

        /* Wait for the threads to finish execution */
        pthread_join(tid[0], 0);
        pthread_join(tid[1], 0);
        return 0;
}
