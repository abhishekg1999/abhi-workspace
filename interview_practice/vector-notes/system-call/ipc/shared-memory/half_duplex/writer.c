#include <stdio.h>
#include <sys/shm.h>

#define key 12  //unique identifier of shared memory
#define size 100  //size of shared memory

int main(){


        int id = shmget(key, size, IPC_CREAT|0666);  //create shared memory sagment
        perror("shmget");

        char *p = shmat(id, 0, 0);  //attached shared memory with process virtual adds space
        perror("shmat");

        printf("Enter data:\n");
        scanf("%s", p);

        shmdt(p);  //detach shared memory
        return 0;
}

