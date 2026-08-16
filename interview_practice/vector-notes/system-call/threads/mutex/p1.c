/*
pthread_mutex_t lock;
pthread_mutex_init(&lock,0);

pthread_mutex_lock(&lock);
pthread_mutex_unlock(&lock);

pthread_mutex_destroy(&lock);
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>  //exit(1)
#include <unistd.h>  //read, write
#include <fcntl.h>   //open

int fd;
pthread_mutex_t lock;

void* thread1(void *p){

	char ch;
	
	pthread_mutex_lock(&lock);  //lock
	for(ch='a'; ch<='z'; ch++)
		write(fd, &ch, 1);

	pthread_mutex_unlock(&lock); //unlock

}

void* thread2(void *p){

        char ch;

	pthread_mutex_lock(&lock);   //lock
        for(ch='A'; ch<='Z'; ch++)
                write(fd, &ch, 1);

	pthread_mutex_unlock(&lock);  //unlock

}

int main(){

	fd = open("file", O_CREAT|O_WRONLY|O_APPEND, 0666);
	if(fd<0){
		perror("open");
		exit(1);
	}

	/* mutex init */
	pthread_mutex_init(&lock, 0);

	/* create thread */
	pthread_t tid[2];  //thread id
	pthread_create(&tid[0], 0, thread1, 0);
	pthread_create(&tid[1], 0, thread2, 0);

	/* Wait for the threads to finish execution */
	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	close(fd);  //close file fd
	pthread_mutex_destroy(&lock);  //delete mutex
	return 0;
}

