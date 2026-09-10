#include <stdio.h>
#include <sys/msg.h>
#include <string.h>

#define key 1

typedef struct {
	long mtype;
	char data[20];
}msgbuf;

int main(){

	msgbuf v;
	v.mtype = 10;

	int mid = msgget(key, IPC_CREAT|0666);
	perror("msgget");

	while(1){
	
		/* recieve */	
		msgrcv(mid, &v, sizeof(v.data), 10, 0);  //recive msg with type 10
		printf("Recieved: %s\n",v.data);

		/* send */
		printf("Send data: \n");
    		scanf("%s", v.data);
    		v.mtype = 20; // send msg with type 20
    		msgsnd(mid, &v, strlen(v.data)+1, 0);

	}

	return 0;
}
