/* 
 * pipe) is  half-duplex, so for two-way communication you must create two pipe s
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {

	int p1[2]; // parent -> child
	int p2[2]; // parent <- child 
	
	pipe(p1);
	pipe(p2);
	perror("pipe");

	if( fork()==0 ){

		char c_buf[20];
		close(p1[1]); //close write end for p1
		close(p2[0]); //close read end for p2
		
		while(1) {

			/* child read from pipe1 */
			read(p1[0], c_buf, sizeof(c_buf));
			printf("Child received: %s\n", c_buf);

			/* child write to pipe2 */
			printf("Enter message from child:\n");
			scanf("%19s", c_buf);
			write(p2[1], c_buf, strlen(c_buf)+1);
		}

		close(p1[0]);  //close read end for p1
		close(p2[1]);  //close write end for p2
	} else {
		
		char p_buf[20];
		close(p1[0]); //close read end for p1
                close(p2[1]); //close write end for p2

		while(1){
			
			/* parent write to pipe1 */
			printf("Enter message from parent:\n");
                        scanf("%19s", p_buf);
                        write(p1[1], p_buf, strlen(p_buf)+1);

			/* parent read from pipe2 */
			read(p2[0], p_buf, sizeof(p_buf));
                        printf("parent received: %s\n", p_buf);

		}

		close(p1[1]);  //close write end for p1
        	close(p2[0]);  //close read end for p2
	}

	return 0;
}

/*

Parent                           Child

scanf()

write(p1[1])
------------------------------>

                              read(p1[0])

                              printf()

                              scanf()

                              write(p2[1])

<------------------------------

read(p2[0])

printf()

(repeat)

*/
