#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void main(void)
{
	char buf[20],op;
	int fd = open("/proc/etx/etx_proc", O_RDWR);
	printf("select option...\n");
	printf("1)write 2)read\n");

	scanf("%c",&op);
	if(op=='1'){
	printf("enter the data \n");
	scanf("%s",buf);
	write(fd, buf, sizeof(buf));
	printf("writing done ..\n");
	}

	else{
	read(fd,buf, sizeof(buf));
	printf("read data =%s\n",buf);
	}
	close(fd);
}
