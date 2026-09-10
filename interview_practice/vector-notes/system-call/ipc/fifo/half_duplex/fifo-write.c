#include <stdio.h>
#include <unistd.h> //for read,write 
#include <string.h> //for strlen
#include <sys/stat.h>  //for mkfifo
#include <fcntl.h>   //for open

int main() {
	
	int fd;
	char buff[10];

	mkfifo("f1",0666);
	perror("mkfifo");
	printf("waiting for read\n");
	fd=open("f1", O_WRONLY);  //If no reader has opened the FIFO, this call blocks.

	
	while(1){
		printf("enter the data\n");
		scanf("%s", buff);
		write(fd, buff, strlen(buff)+1);

	}

	close(fd);
	return 0;
}
