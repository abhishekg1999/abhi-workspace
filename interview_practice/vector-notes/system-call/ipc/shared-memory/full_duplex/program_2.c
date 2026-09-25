#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define key 5

typedef struct {
    char buf1[20];
    char buf2[20];
} shm_t;

int shmid;
int semid;
shm_t *p;

/* lock api */
void sem_wait(int id, int semno) {

    struct sembuf op = {semno, -1, 0};
    semop(id, &op, 1);
}

/* unlock api */
void sem_signal(int id, int semno) {

    struct sembuf op = {semno, +1, 0};
    semop(id, &op, 1);
}

int main() {

	shmid = shmget(key, sizeof(shm_t), IPC_CREAT|0666);
	p = shmat(shmid, 0, 0);

	semid = semget(key, 2, IPC_CREAT|0666);

	while(1) {

		sem_wait(semid, 1);  //p2 waiting for p1
		printf("Received: %s\n", p->buf1);

		printf("Enter data: \n");
        	scanf("%19s", p->buf2);
		sem_signal(semid, 0);   //unlock p1
 
	}
}
