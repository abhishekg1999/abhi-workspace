
## APIS 
write:
mkfifo("fifo", 0666);
fd = open("fifo", O_WRDONLY);
write(fd[0], buff, strlen(buff)+1);

read:
fd = open("fifo", O_RDDONLY);
read(fd[1], buff, sizeof(buff));
