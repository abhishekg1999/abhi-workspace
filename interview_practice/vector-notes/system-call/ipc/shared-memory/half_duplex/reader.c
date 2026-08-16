#include <stdio.h>
#include <sys/shm.h>

#define key 12  //unique identifier of shared memory
#define size 100  //size of shared memory

int main(){


        int shmid = shmget(key, size, IPC_CREAT|0666); //create shared memory sagment
        perror("shmget");

        char *p = shmat(shmid, 0, 0);  //attached shared memory with process virtual adds space
        perror("shmat");

        printf("shared memory data: %s\n", p);

        shmdt(p);   //detach shared memory
        shmctl(shmid, IPC_RMID, NULL);  //delete shared memory
        return 0;
}
