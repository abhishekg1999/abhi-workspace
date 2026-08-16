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

	fd1 = open("f1", O_RDONLY); //for read
	fd2 = open("f2", O_WRONLY);  // for write

	while(1) {

		/* read from f1 */
                read(fd1, buf, sizeof(buf));
                printf("Recieved data : %s\n", buf);

		/* write to f2 */
		printf("enter the data\n");
		scanf("%s", buf);
		write(fd2, buf, strlen(buf)+1);

	}

	close(fd1);
	close(fd2);
	return 0;
}
