#include <stdio.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>

#define key   2   //semaphore key
#define nsem  5  //create array of 5 semaphore

void sem_wait(int semid, int semno) {
    
    struct sembuf op = { 
    	.sem_num = semno,   //semaphore number
    	.sem_op = -1,       // set operation
    	.sem_flg = 0    
    };
    
    semop(semid, &op, 1);
}

void sem_signal(int semid, int semno) {

    struct sembuf op = { 
    	.sem_num = semno, //semaphore number
    	.sem_op = 1,      // set operation
    	.sem_flg = 0 
    };
    
    semop(semid, &op, 1);
}

int main()
{
	char ch;

	int fd=open("file",O_WRONLY|O_CREAT|O_APPEND,0666);
	if(fd<0) {
		perror("open");
		return 1;
	}
	
	int semid=semget(key, nsem, IPC_CREAT|0666);  //creat 5 semaphores
	if(semid<0) {
		perror("semget");
		return 1;
	}

	sem_wait(semid, 2);  //wait for p1 to finish to lock p2
	printf("p2 writing data\n");
	for(ch='A';ch<='Z';ch++) {
		
		write(fd,&ch,1);
		sleep(1);
	}
	
	sem_signal(semid, 2);   //unlock p2
	printf("data write complete\n");


	close(fd);
	semctl(semid, 0, IPC_RMID);  //delete semaphore
	return 0;
}

