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

<unistd.h>
pipe()
read(), write(), sleep()

<sys/socket.h>
socketpair()

<sys/stat.h>
mkfifo()

<fcntl.h>
open(), fcntl()

<sys/msg.h>
msgget(), msgsnd(), msgrcv(), msgctl()

<stdlib.h>
exit(), malloc()

<signal.h>
signal()

<sys/shm.h>
shmget(), shmat(), shmdt(), shmctl()
