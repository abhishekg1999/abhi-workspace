######## follow ########

- pipe
- socketpair
- fifo
- msgQ
- shared memory


Pipe          → Half-duplex 
FIFO          → Half-duplex
socketpair    → Full-duplex 
Message Q     → Full-duplex
Shared Memory → Full-duplex 

## used header files

#include<unistd.h>
pipe()
read(), write(), sleep()

#include<fcntl.h>
open(), fcntl()

#include<stdlib.h>
exit(), malloc()

#include<signal.h>
signal()

#include<sys/socket.h>
socketpair()

#include<sys/stat.h>
mkfifo()

#include<sys/msg.h>
msgget(), msgsnd(), msgrcv(), msgctl()

#include<sys/shm.h>
shmget(), shmat(), shmdt(), shmctl()
