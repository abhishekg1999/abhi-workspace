#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
fcntl(fd,F_SETLKW,&v);
fd 	--> file descripter
F_SETLKW  -> flag
&v  	--> structure variable */

int fd;
void record_lock(void){
	
	struct flock v = {
		.l_type = F_WRLCK,    //set lock flag
        	.l_whence = SEEK_SET, //set fd to begining
        	.l_start =0,
        	.l_len = 0
	};

	fcntl(fd, F_SETLKW, &v);
}

void record_unlock(void){

        struct flock v = {
        	.l_type = F_UNLCK,   //clear lock flag
        	.l_whence = SEEK_SET, //set fd to begining
        	.l_start =0,
        	.l_len = 0
	};

        fcntl(fd, F_SETLKW, &v);
}

int main(){

	char ch;
	fd = open("file", O_RDWR|O_CREAT|O_APPEND, 0666);


	record_lock();  //lock p1
	printf("p1 writing data\n");
	for(ch='a'; ch <='z'; ch++){
		write(fd, &ch, 1);
		sleep(1);
	}
	
	record_unlock();  //unlock p1
	printf("data write ..done\n");

	close(fd);
	return 0;
}

