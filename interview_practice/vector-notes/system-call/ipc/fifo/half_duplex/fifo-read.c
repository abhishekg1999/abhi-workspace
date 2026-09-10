#include <stdio.h>
#include <unistd.h> //for read,write 
#include <string.h> //for strlen
#include <sys/stat.h>  //for mkfifo
#include <fcntl.h>   //for open

int main()
{
	int fd;
	char buff[10];

	fd=open("f1", O_RDONLY);
	perror("open");

	while(1)
	{
		read(fd, buff, sizeof(buff));
		printf("read data=%s\n",buff);
	}

	close(fd);
	return 0;
}
