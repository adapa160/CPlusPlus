#include <sys/types.h>          /* file of data types needed for many compilers */
#include <unistd.h>             /* needed for fork, getpid procedures */
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main (void)
{

 char buffer[256];
 sprintf(buffer, "Hello World !\n\n");
 pid_t pid;              /* variable to record process id of child */

 printf("\n\n******\n* RTS Lab *\n******\n\n");

 pid = fork();           /* create new process */
 if ( -1 == pid)         /* FIRST IF-STATEMENT */
 { 
  perror ("error in fork");  
  exit (1);
 }


///Both wait both in sleep
//parent sleep child in zombie
//Zombie process but sub i process

int k=20;
if(pid != 0){
//sleep(3);
printf("%s\n",buffer);

sprintf(buffer,"This is the the parent process!\n");

printf("%s\n",buffer);

sprintf(buffer,"parent report: my pid =");
printf("%s %d \n",buffer,getpid());
sprintf(buffer,"Child report: child pid =");
printf("%s %d \n",buffer,pid);

int i=0;

while(i < 20){
sleep(1);

k=20-i;
printf("%d \n",k);
waitpid(pid,NULL,0);
i++;
}

}


if (pid == 0){
sleep(3);
printf("%s\n",buffer);
sprintf(buffer,"This is the child process!");

printf("%s\n",buffer);

sprintf(buffer,"child report: my pid =");
printf("%s %d \n",buffer,getpid());



//sleep(3);


//printf("%d \n", status);

printf("Countdown starts.......\n");
}



 /* End of assignment 3.1 a) */

 exit (0);               /* quit by reporting no error */
 }

