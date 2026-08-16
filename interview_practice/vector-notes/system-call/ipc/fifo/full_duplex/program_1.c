/*
 * FIFO (named pipe) is also half-duplex, so for two-way communication you must create two FIFOs
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){

	int fd1, fd2;
	char buf[10];

	mkfifo("f1", 0666);
	mkfifo("f2", 0666);
	perror("mkfifo");
	printf("waiting for read\n");

	fd1 = open("f1", O_WRONLY); //for write
	fd2 = open("f2", O_RDONLY);  // for read

	while(1) {

		/* write to f1 */
		printf("enter the data\n");
		scanf("%s", buf);
		write(fd1, buf, strlen(buf)+1);

		/* read from f2 */
		read(fd2, buf, sizeof(buf));
		printf("Recieved data : %s\n", buf);

	}

	close(fd1);
	close(fd2);
	return 0;
}
