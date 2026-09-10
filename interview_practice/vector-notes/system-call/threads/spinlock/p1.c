/*
pthread_spinlock_t lock;
pthread_spin_init(&lock,0);

pthread_spin_lock(&lock);
pthread_spin_unlock(&lock);

pthread_spin_destroy(&lock);
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>  //read, write
#include <fcntl.h>   //open

int fd;
pthread_spinlock_t lock;

void* thread1(void *p){

	char ch;
	
	pthread_spin_lock(&lock);  //lock
	for(ch='a'; ch<='z'; ch++)
		write(fd, &ch, 1);

	pthread_spin_unlock(&lock); //unlock

}

void* thread2(void *p){

        char ch;

	pthread_spin_lock(&lock);   //lock
        for(ch='A'; ch<='Z'; ch++)
                write(fd, &ch, 1);

	pthread_spin_unlock(&lock);  //unlock

}

int main(){

	fd = open("file", O_CREAT|O_WRONLY|O_APPEND, 0666);
	if(fd<0){
		perror("open");
		return -1;
	}

	/* spin lock init */
	pthread_spin_init(&lock, 0);

	/* create thread */
	pthread_t tid[2];  //thread id
	pthread_create(&tid[0], 0, thread1, 0);
	pthread_create(&tid[1], 0, thread2, 0);

	/* Wait for the threads to finish execution */
	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	close(fd);  //close file fd
	pthread_spin_destroy(&lock);  //delete spinlock
	return 0;
}

