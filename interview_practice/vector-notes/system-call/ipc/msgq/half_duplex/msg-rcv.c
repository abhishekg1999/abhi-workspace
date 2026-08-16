#include <stdio.h>
#include <sys/msg.h>

typedef struct {
	long mtype;
	char data[20];
}msgbuf;

int main(){

	msgbuf v;
	v.mtype = 10; //same as sender

	int mid = msgget(1, IPC_CREAT|0666);
	if(mid<0){

		perror("msgget");
		return 1;
	}

	printf("received data ...\n");

	while(1){

		msgrcv(mid, &v, sizeof(v.data), v.mtype, 0); //block till data send from msgsnd
		printf("data=%s\n",v.data);
	}

	return 0;
}
