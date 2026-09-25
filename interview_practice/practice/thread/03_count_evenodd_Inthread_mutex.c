/*
mutex => "its say Only one thread can access count at a time"
mutex + condition => "it says Only one thread can access count at a time, AND each thread must wait until it is actually its turn"
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int count=0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;   

/* print even count */
void* thread1(void *p){

	while(1){
		
		pthread_mutex_lock(&lock);

		/* Wait until count is odd */	
		while(count%2 !=0 && count<10)
			pthread_cond_wait(&cond, &lock);  //if count is odd , thread1 goto sleep so thread2 can print odd count

		if(count >=10){
			pthread_cond_signal(&cond);
                        pthread_mutex_unlock(&lock);
                        break;
                }
		
		printf("in thread1 = %d\n", count);
		count++;

		pthread_cond_signal(&cond);  //wakeup thread2
		pthread_mutex_unlock(&lock); //unlock mutex
		sleep(1);
	}

	return 0;
}

/* print odd count */ 
void* thread2(void *p){

	while(1) {

		pthread_mutex_lock(&lock);
		
		/* wait until count is even */
		while(count%2 ==0 && count<10)
                        pthread_cond_wait(&cond, &lock);   //if count is even , thread2 goto sleep so thread1 can print even count

		if(count >=10){
                        pthread_cond_signal(&cond);
                        pthread_mutex_unlock(&lock);
                        break;
                }

		printf("in thread2 = %d\n", count);
                count++;
		
		pthread_cond_signal(&cond);  //wake up thread1 
		pthread_mutex_unlock(&lock); //unlock mutex
		sleep(1);
	}

	return 0;
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
