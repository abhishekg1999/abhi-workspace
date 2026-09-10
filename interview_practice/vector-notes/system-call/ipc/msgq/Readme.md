typedef struct {
	long mtype;
	char data[20];
}msgbuf;

msgbuf v = {
    .mtype= 10,
    .data= "embedded"
};

int mid = msgget(1, IPC_CREAT|0666);  //create
msgsnd(mid, &v, strlen(v.data)+1, 0);  //send
msgrcv(mid, &v, sizeof(v.data), v.mtype, 0);  //recieve
msgctl(mid, IPC_RMID, NULL);    //delete

## check the msg Q 

$ ipcs -q    // show msg Q
$ ipcrm -q <id>  // remove it
