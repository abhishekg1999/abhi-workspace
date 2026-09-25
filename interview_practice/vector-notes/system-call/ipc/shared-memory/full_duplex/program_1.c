#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <signal.h>

#define key 5

typedef struct {
    char buf1[20];
    char buf2[20];
} shm_t;

int shmid;
int semid;
shm_t *p;

/* function used to lock */
void sem_wait(int id, int semno) {

    struct sembuf op = {semno, -1, 0};
    semop(id, &op, 1);
}

/* function used to unlock */
void sem_signal(int id, int semno) {

    struct sembuf op = {semno, +1, 0};
    semop(id, &op, 1);
}

void clean_shm(int sig) {

    shmdt(p); //detach shared memory
    shmctl(shmid, IPC_RMID, NULL); // delete shared memory
    semctl(semid, 0, IPC_RMID);  //delete semaphore 
    
    printf("\nIPC deleted\n");
    exit(0);
}

int main() {

	signal(SIGINT, clean_shm);

	shmid = shmget(key, sizeof(shm_t), IPC_CREAT|0666);
	p = shmat(shmid, 0, 0);
	
	semid = semget(key, 2, IPC_CREAT|0666);  //create semaphore
	semctl(semid, 0, SETVAL, 0);  // Initialize semaphore 0 to 0
	semctl(semid, 1, SETVAL, 0);  // Initialize semaphore 1 to 0

	while(1) {

		printf("Enter the data\n");
		scanf("%19s", p->buf1);
		sem_signal(semid, 1);   //unlock P2
 
		sem_wait(semid, 0);     //p1 waiting for p2
		printf("Recived :%s\n", p->buf2);
	}
}
