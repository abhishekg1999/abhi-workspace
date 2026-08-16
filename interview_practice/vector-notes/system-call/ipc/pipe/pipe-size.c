#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int p[2],count;
	char ch='a';
 
	pipe2(p,O_NONBLOCK);//after pipe full write will fail it will not wait for read
//	pipe2(p,O_DIRECT);  //after pipe full write will wait untill get some space from read
	count=0;
	while(write(p[1],&ch,1)!=-1) 
	count++;

	printf("pipe-size=%d\n",count);
	close(p[0]);
	close(p[1]);
}
