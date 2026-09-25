/* 
 *  this program is using socketpair which is advance of pipe , pipe is one way communication meanwhile socketpair is two way communication or full duplex
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

int main() {

        int fd[2];
	
	/* create connected socket pair */
        socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
        perror("socketpair");

        if( fork()==0 ){

                char b[20];
                close(fd[1]);   //close

                while(1) {

                        /* child read from fd[0] */
                        read(fd[0], b, sizeof(b));
                        printf("child read =%s\n", b);

                        /* child write to fd[0] */
                        printf("scan data from child\n");
                        scanf("%19s", b);
                        write(fd[0], b, sizeof(b));
                }

                close(fd[0]);  //close

        } else {

                char a[20];
                close(fd[0]);   //close

                while(1) {

                        /* parent write to fd[1] */
                        printf("scan data from parent\n");
                        scanf("%19s", a);
                        write(fd[1], a, sizeof(a));

                        /* parent read from fd[1] */
                        read(fd[1], a, sizeof(a));
                        printf("parent read =%s\n", a);
                }

                close(fd[1]);  //close
        }

        return 0;
}

/* 

Parent                          Child

scanf()

write(fd[1])
---------------------------->

                           read(fd[0])

                           printf()

                           scanf()

                           write(fd[0])

<----------------------------

read(fd[1])

printf()
 
 */
