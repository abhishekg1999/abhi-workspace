#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
	
	int p[2];
	pipe(p);
	perror("pipe");
	printf("p[0]=%d\tp[1]=%d\n",p[0],p[1]);

	if(fork()==0) {
		
		//child process reading the data from pipe
		char b[20];
		close(p[1]);  //close write end 
			      
		while(1) {
			read(p[0],b,sizeof(b));   //reading pipe data
			printf("data=%s\n",b);
		}

	} else {
		
		//parent process writing data into pipe
		char a[20];
		close(p[0]);  //close read end
			      
		while(1) {
			printf("enter the data\n");
			scanf("%19s",a);   //only scan 19 characters as size of a is 20 [19 char +\0]
			write(p[1],a,sizeof(a));
		}
	}

	return 0;
}

/* 

Parent                         Child
			      
scanf()

write(p[1])
---------------------------->

                          read(p[0])

                          printf()

scanf()

write()

---------------------------->

                          read()

                          printf()
                          
*/
