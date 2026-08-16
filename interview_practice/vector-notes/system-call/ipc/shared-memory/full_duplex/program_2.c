#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define key 5

typedef struct {
    char buf1[20];
    char buf2[20];
} shm_t;

/* lock api */
void sem_wait(int semid, int semno) {

    struct sembuf op = {semno, -1, 0};
    semop(semid, &op, 1);
}

/* unlock api */
void sem_signal(int semid, int semno) {

    struct sembuf op = {semno, +1, 0};
    semop(semid, &op, 1);
}

int main() {

	int shm_id = shmget(key, sizeof(shm_t), IPC_CREAT|0666);
	shm_t *p = shmat(shm_id, NULL, 0);

	int sem_id = semget(key, 2, IPC_CREAT|IPC_EXCL|0666);
	sem_id = semget(key, 2, 0666);

	
	while(1) {

		sem_wait(sem_id, 1);  //p2 waiting for p1
		
		printf("Received: %s\n", p->buf1);

		printf("Enter data: \n");
        	scanf("%19s", p->buf2);
		sem_signal(sem_id, 0);   //unlock p1
 
	}
}
