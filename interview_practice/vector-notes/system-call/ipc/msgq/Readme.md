typedef struct {
	long mtype;
	char data[20];
}msgbuf;

msgbuf v = {
    .mtype= 10,
    .data= "embedded"
};

int mid = msgget(1, IPC_CREAT|0666);
msgsnd(mid, &v, strlen(v.data)+1, 0);
msgrcv(mid, &v, sizeof(v.data), v.mtype, 0);

## check the msg Q 

$ ipcs -q    // show msg Q
$ ipcrm -q <id>  // remove it
