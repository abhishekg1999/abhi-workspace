/* 
 *  this program is using socketpair which is advance of pipe , pipe is one way communication meanwhile socketpair is two way communication
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

                char c_buff[20];
                close(fd[1]);   //close

                while(1) {

                        /* child read from fd[0] */
                        read(fd[0], c_buff, sizeof(c_buff));
                        printf("child read =%s\n", c_buff);

                        /* child write to fd[0] */
                        printf("scan data from child\n");
                        scanf("%19s", c_buff);
                        write(fd[0], c_buff, sizeof(c_buff));
                }

                close(fd[0]);  //close

        } else {

                char p_buff[20];
                close(fd[0]);   //close

                while(1) {

                        /* parent write to fd[1] */
                        printf("scan data from parent\n");
                        scanf("%19s", p_buff);
                        write(fd[1], p_buff, sizeof(p_buff));

                        /* parent read from fd[1] */
                        read(fd[1], p_buff, sizeof(p_buff));
                        printf("parent read =%s\n", p_buff);
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
