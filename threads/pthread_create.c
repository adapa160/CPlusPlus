/* Pthread creation and termination */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS     5
pthread_cond_t cond;
pthread_mutex_t mutex;

void *PrintHello(void *threadid)
{
	long tid;
	tid = (long)threadid;


while(1){
int a= pthread_cond_wait(&cond,&mutex);
        if(!a){
        printf("Condition satisfied entering printhello execution");  
break;
}
}



}


int main (int argc, char *argv[])
{

	pthread_t threads[NUM_THREADS];
	long t;
pthread_mutex_init(&mutex,NULL);

	for(t=0; t<NUM_THREADS; t++){


		
pthread_mutex_lock(&mutex);
printf("Main program: creating thread number<%ld\n\n",t);
pthread_create(&threads[t],NULL,&PrintHello,&t);
pthread_join(threads[t],NULL);
pthread_cond_signal(&cond);
pthread_mutex_unlock(&mutex);



	}


	printf("Main program: Completed. Exiting\n");
        pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);
}
