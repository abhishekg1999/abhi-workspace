#include <stdio.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

typedef struct {
	long mtype;
	char data[20];
}msgbuf;

int mid;

/* delete the msg Q*/
void clean_msgQ(int signum){

	if (msgctl(mid, IPC_RMID, NULL) == 0) {
        	printf("Message queue deleted successfully.\n");
    	} 
	else {
        	perror("Failed to delete message queue\n");
    	}

	exit(0);
}

int main(){

	
	signal(SIGINT, clean_msgQ);  /* just to delete the created msg Q with cntrl+C */

	msgbuf v;
	v.mtype = 10;

	mid = msgget(1, IPC_CREAT|0666);
	if(mid<0){

		perror("msgget");
		return 1;
	}

	while(1) {
		
		/* send */
		printf("Send data:\n");
        	scanf("%s", v.data);
		v.mtype = 10;   //send msg with type 10
		msgsnd(mid, &v, strlen(v.data)+1, 0);

		/* recieve */
		msgrcv(mid, &v, sizeof(v.data), 20, 0); // recieve msg with type 20
    		printf("Received: %s\n", v.data);
	}

	return 0;
}
