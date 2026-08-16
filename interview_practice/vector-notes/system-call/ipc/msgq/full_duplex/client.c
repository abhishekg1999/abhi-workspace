#include <stdio.h>
#include <sys/msg.h>
#include <string.h>

typedef struct {
	long mtype;
	char data[20];
}msgbuf;

int main(){

	msgbuf v;
	v.mtype = 10;

	int mid = msgget(1, IPC_CREAT|0666);
	if(mid<0){

		perror("msgget");
		return 1;
	}

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
