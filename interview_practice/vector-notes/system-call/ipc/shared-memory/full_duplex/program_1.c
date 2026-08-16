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

int shm_id;
int sem_id;
shm_t *p;

/* function used to lock */
void sem_wait(int semid, int semno) {

    struct sembuf op = {semno, -1, 0};
    semop(semid, &op, 1);
}

/* function used to unlock */
void sem_signal(int semid, int semno) {

    struct sembuf op = {semno, +1, 0};
    semop(semid, &op, 1);
}

void clean_shm(int sig) {

    shmdt(p); //detach shared memory
    shmctl(shm_id, IPC_RMID, NULL); // delete shared memory
    semctl(sem_id, 0, IPC_RMID);  //delete semaphore 
    
    printf("\nIPC deleted\n");
    exit(0);
}

int main() {

	signal(SIGINT, clean_shm);

	shm_id = shmget(key, sizeof(shm_t), IPC_CREAT|0666);
	p = shmat(shm_id, NULL, 0);
	
	sem_id = semget(key, 2, IPC_CREAT|IPC_EXCL|0666);  //create semaphore
	semctl(sem_id, 0, SETVAL, 0);  // Initialize semaphore 0 to 0
	semctl(sem_id, 1, SETVAL, 0);  // Initialize semaphore 1 to 0

	while(1) {

		printf("Enter the data\n");
		scanf("%19s", p->buf1);

		sem_signal(sem_id, 1);   //unlock P2
 
		sem_wait(sem_id, 0);     //p1 waiting for p2
		printf("Recived :%s\n", p->buf2);
	}
}
