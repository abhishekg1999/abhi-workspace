#include <stdio.h>
#include <pthread.h>

void* thread1(void *p){

        printf("in thread1 msg=%s\ttid=%ld\n",(char*)p,pthread_self());
}

void* thread2(void *p){

        printf("in thread2 msg=%s\ttid=%ld\n",(char*)p,pthread_self());
}

int main(){

        pthread_t tid[2];  //thread id

        pthread_create(&tid[0], 0, thread1, "hi");
        pthread_create(&tid[1], 0, thread2, "bye");

        /* Wait for the threads to finish execution */
        pthread_join(tid[0], 0);
        pthread_join(tid[1], 0);
        return 0;
}

