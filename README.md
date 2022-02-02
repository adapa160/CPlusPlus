# RTOS Files
The following upload contains two section of files:
Files pertaining to Process threads and Muted-----------
1. Process
-----------------------

a. Got a detailed view of Parent Process ID and Process ID and their correlation.
b. Didnt  see  it because it is terminated before ps xlf executed.
c. Parent state is  s+, 
Child state is s+ 
s  depicts  waiting for an event to complete,
+  depicts    running in foreground process group
d. Parent :--  s+, child state:--  z+ (z+ - Indicates its a zombie process yet to be collected by internal garbage collector which is not operational at the moment.
e. Parent state :-- s+,
 Child State:-- s
f. Child Process :--
 S+ for 3 seconds,
 Child State:--
 z+ till 20s.
g. Wait PID function waits to make sure the parent waits till child executes.


2. Threads
----------------------
a. pThread_create used to create and passed necessary arguments for the same.
b. Compared the execution order and creation order.



3. Mutex
----------------------
a. Multiple threads have been using the same resource the problem of which has been solved using Mutex and conditional Synchronisation.
b. Identified the critical section of the code by considering the shared resource.
c. cond_wait() and cond_signal() to suspend and wake up the suspended thread.
   mutex_lock() and mutex_unlock()
   
   
   
   
DEVICES:
Drivers written for teensy hardware which plays music and analyses the frequency distribution and based on this varies the LED display
