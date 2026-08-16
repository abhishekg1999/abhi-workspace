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

	printf("enter the data\n");
	while(1) {

        	scanf("%s", v.data);
		msgsnd(mid, &v, strlen(v.data)+1, 0);
	}

	return 0;
}
