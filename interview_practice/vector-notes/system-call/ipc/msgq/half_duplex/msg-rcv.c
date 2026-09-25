#include <stdio.h>
#include <sys/msg.h>

#define key 1

typedef struct {
	long mtype;
	char data[20];
}msgbuf;

int main(){

	msgbuf v;
	v.mtype = 10; //same as sender

	int mid = msgget(key, IPC_CREAT|0666);
	perror("msgget");


	msgrcv(mid, &v, sizeof(v.data), v.mtype, 0); //block till data send from msgsnd
	printf("data=%s\n",v.data);

	msgctl(mid, IPC_RMID, NULL);  //delete msgQ
	return 0;
}
