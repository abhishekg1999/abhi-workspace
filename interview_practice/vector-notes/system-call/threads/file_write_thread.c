#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>  //exit(1)
#include <unistd.h>  //read, write
#include <fcntl.h>   //open

int fd;

void* thread1(void *p){

	char ch;
	for(ch='a'; ch<='z'; ch++)
		write(fd, &ch, 1);

}

void* thread2(void *p){

        char ch;
        for(ch='A'; ch<='Z'; ch++)
                write(fd, &ch, 1);

}

int main(){

	fd = open("file", O_CREAT|O_WRONLY|O_APPEND, 0666);
	if(fd<0){
		perror("open");
		exit(1);
	}

	pthread_t tid[2];  //thread id
	pthread_create(&tid[0], 0, thread1, 0);
	pthread_create(&tid[1], 0, thread2, 0);

	/* Wait for the threads to finish execution */
	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);
	return 0;
}

