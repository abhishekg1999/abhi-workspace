
    flow
#writer program:
shmget → creates memory in kernel
shmat  → attaches to it
write  → data stored in kernel memory
shmdt  → writer detaches

note: if Writer not attached but Shared memory still exists in kernel

#reader program:
shmget → gets SAME existing segment
shmat  → attaches to it
read   → sees writer data
shmdt  → reader detaches


## APIS 

#define key 12  //unique identifier of shared memory
#define size 100  //size of shared memory

int shmid = shmget(key, size, IPC_CREAT|0666); //create shared memory sagment
char *p = shmat(shmid, 0, 0);  //attached shared memory with process virtual adds space
shmdt(p);   //detach shared memory
shmctl(shmid, IPC_RMID, NULL);  //delete shared memory

## check the shared memory 

$ ipcs -m    // show shared memory
$ ipcrm -m <id>  // remove it
