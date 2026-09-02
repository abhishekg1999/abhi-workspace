#include <stdio.h>
#include <pthread.h>

int counter = 0;
void *thread_func(void *arg) {

	for (int i = 0; i < 100000; i++)
		counter++;

	return NULL;
}

int main() {

	pthread_t tid[2];
	pthread_create(&tid[0], 0, thread_func, 0);
	pthread_create(&tid[1], 0, thread_func, 0);

	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	printf("count =%d\n", counter);
	return 0;
}

/*
Explanation:
Even though counter++ looks like a single step in C, the compiler translates it into three separate assembly instructions for the CPU:

Read: Fetch the current value of counter from RAM into a CPU register.
Modify: Add 1 to the register value.
Write: Copy the updated value from the register back into RAM

1. Thread 1 reads the fresh value 10 from RAM into its register.
2. The operating system context-switches to Thread 2.
3. Thread 2 reads the exact same fresh value 10 from RAM
4. Thread 2 increments it to 11 and writes 11 back to RAM
5. The operating system switches back to Thread 1.
6. Thread 1 increments its register value (10 + 1 = 11) and writes 11 back to RAM.

Result: Two increments occurred, but the value in RAM only increased by 1. The work done by Thread 2 was completely erased!

conclusion:
Because the operating system can switch between your two threads at any time, their steps will overlap and overwrite each other:
*/
