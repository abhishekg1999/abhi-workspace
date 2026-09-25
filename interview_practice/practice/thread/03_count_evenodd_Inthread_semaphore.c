#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

//semaphore variable 
sem_t sem1;
sem_t sem2;
int count=0;

/* print even count */
void* thread1(void *p){

	while(1){
	
		sem_wait(&sem1);
		if(count>10){
			sem_post(&sem2);
			break;
		}	
		
		printf("in thread1 = %d\n", count);
		count++;
		
		sem_post(&sem2);
		sleep(1);
	}

	return 0;
}

/* print odd count */
void* thread2(void *p){

        while(1){

                sem_wait(&sem2);
                if(count>10){
                        sem_post(&sem1);
                        break;
                }

                printf("in thread2 = %d\n", count);
                count++;

                sem_post(&sem1);
                sleep(1);
        }

        return 0;
}

int main(){

        pthread_t tid[2];  //thread id
	
	// Initialize semaphores:
    	// sem1 starts at 1 so Thread 1 (Even) can immediately run.
    	// sem2 starts at 0 so Thread 2 (Odd) is forced to wait initially.
	sem_init(&sem1, 0, 1);
        sem_init(&sem2, 0, 0);

        pthread_create(&tid[0], 0, thread1, 0);
        pthread_create(&tid[1], 0, thread2, 0);
        pthread_join(tid[0], 0);
        pthread_join(tid[1], 0);

	// Clean up semaphore resources
	sem_destroy(&sem1);
	sem_destroy(&sem2);
        return 0;
}

/*
1. set sem1 to 1 and sem2 to 0 using sem_init();
2. thread2 sleeping, thread1 => sem1 is 1 (thread1 runs and decrement sem1 to 0) => print count even value => set sem2 to 1 via sem_post(&sem2)  && wake up thread2
3. thread1 sleeping, thread2 => sem2 is 1 (thread2 runs and decrement sem2 to 0) => print count odd value => set sem1 to 1 via sem_pos(&sem1) && wake up thread1
4. repeat ...

*/
