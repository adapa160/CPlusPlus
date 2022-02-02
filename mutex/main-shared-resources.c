/* necessary headers */
#include <assert.h> 
#include <unistd.h> 
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include <pthread.h>

/* the number of shared resources */
#define RESOURCES 3

/* the number of threads wanting to use these resources, must be an even number */
#define THREADS   14

/* how long run this program (in seconds) */
#define TIMEOUT 120


/* the following functions are provided: */

/** Initialize graphic window. */
void init_graphics(void);

/** Cleanup graphics. */
void exit_graphics(void);

/** Sleep for the given number of milliseconds. */
void millisleep(unsigned int ms);

/** Return the number of milliseconds since program start */
int getmillisec(void);

/** Mark the given thread as idle, used for displaying and status printing. */
void set_idle(int thread);

/** Mark the given thread as waiting for a resource, used for displaying and status printing. */
void set_waiting(int thread);

/** Mark the given thread as busy using the given resource, used for displaying and status printing. */
void set_busy(int resource, int thread);


/* include a file containing helper functions */
#include "graphics.c"


/** This array stores has an entry for each resource, if the int is 0
  it means the resource is free, if it is set to 1, it means the resource is used. */
int resources[RESOURCES];


void* thread_mainloop(void* data)
{
   /* per-thread initialization */
   int thread_index = (int)data;
   time_t start = time(0);
   fprintf(stderr, "running %d\n", thread_index);
   set_idle(thread_index);
   millisleep(10 + rand() % 10000);

   /* run until the timeout is over */
   while (time(0) - start < TIMEOUT)
   {
      int free_resource = -1;

      /* try to find a free resource */
      while(free_resource == -1)
      {
         int i = 0;
         for (i=0; i<RESOURCES; i++)
         {
            if (resources[i]==0)
            {
               free_resource = i;
               break;
            }
         }

         /* if a free resource was found, use it, otherwise wait */
         if (free_resource != -1)
         {
            fprintf(stderr, "[%d ms] > %d uses resource %d\n", getmillisec(), thread_index, free_resource);
            resources[free_resource] = 1;
            set_busy(free_resource, thread_index);
         }
         else
         {
            set_waiting(thread_index);
         }
      }

      /* if we get here, we have found a free resource, so now we occupy it for some time */
      millisleep(3000 + rand() % 2000);
      
      /* after that we free the resource again and go back to idle state for some time */
      fprintf(stderr, "[%d ms] < %d releases resource %d\n", getmillisec(), thread_index, free_resource);
      
      resources[free_resource] = 0;

      set_idle(thread_index);
      millisleep(3000 + rand() % 1000);
   }
   return 0;
}


int main(void)
{
   /* Initialization */
   int i=0;
   init_graphics();
   srand(time(0));

   for (i=0; i<RESOURCES; i++)
   {
      resources[i]=0;
   }

   /* Create the threads and run them */
   pthread_t threads[THREADS];
   for (i=0; i<THREADS; i++)
   {
      pthread_create(&threads[i], NULL, &thread_mainloop, (void*)i);
   }

   /* Let them run for the given number of seconds */
   sleep(TIMEOUT);

   /* and finally exit */
   exit_graphics();
   return 0;
}

