#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* thread(void *p)
{
	printf("in thread -- msg=%s\n",(char*)p);
	sleep(1);
	pthread_exit("bye"); //return to calling process and exit form thread
}

int main()
{
	pthread_t t1;
	void *p;
	
	pthread_create(&t1,0,thread,"abhishek"); //create thread
	pthread_join(t1,&p);		//waiting for thread with tid t1 and collect the return value from thread
	printf("return from thread =%s\n",(char*)p);
}
