## APIS 

int fd[2];
socket(AF_UNIX, SOCK_STREAM, 0, fd);
read(fd, buff, strlen(buff)+1);
write(fd, buff, sizeof(buff));
