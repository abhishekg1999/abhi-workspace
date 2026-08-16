#include <stdio.h>
#include <unistd.h>		//for pipe,dup,dup2,execlp
 #include <sys/wait.h>  //for wait
int main()
{
	int p[2];
	pipe(p);

	if(fork()==0)
	{
		//execute ls command child1
		close(p[1]);//write end close
		close(p[0]);//read end close
		dup(p[1]);
		execlp("ls","ls",(char*)NULL);
	}
	else
	{
		if(fork()==0)
		{	
			//execute wl -l command child2
			close(p[1]);//close write end
			dup2(p[0],0);
			execlp("wc","wc","-l",(char*)NULL);
			
		}
		else
		{
			close(p[0]);//closed read end
			close(p[1]);//closed write end
			wait(0);//wait for child1 to finish
			wait(0);//wait for child2 to finish

		}
	}
}
