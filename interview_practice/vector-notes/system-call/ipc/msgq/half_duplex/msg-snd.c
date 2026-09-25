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

		
	printf("enter the data\n");
        scanf("%s", v.data);
	msgsnd(mid, &v, strlen(v.data)+1, 0);

	return 0;
}
