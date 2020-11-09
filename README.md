# threaded-producer-consumer
## Thread Synchronization
### Rose Ramirez, Daus Carmichael


#### Abstract
This project uses the producer-consumer model to demonstrate how threads can be and must be synchronized.
Using the `pthread` library which contains thread behaivor, thread mutexes and the basic attribute initilization methods this provides a way to create threads and utilize them in C.  

#### Structure
The code is run with the syntax **./prodcon \<nitems\>** where nitems is an integer. The bounds on n are $0\geq n \leq 1365$. This is derived from setting 64KB(65536 bytes) as the maximum size of the buffer and calculated by `MEMSIZE/sizeof(item)`. Following the memory allocation to the `buffer`, a thread is created to consume the data. The semaphore, however, is set to ensure that the data is produced first. Then the producer and the consumer will fight for the mutex and counting semaphore. This could be more efficient using `try` and perform a seperate task, but for this assignment the code will suffice. Once the code reaches completion of n items, it exits will status code 0.
#### Procedure
 1. **./prodcon <nitems>**
 2. check arguments are valid
 3. create thread attributes & mutex attributes
 4. start thread `consumer`
 5. run each loop `n` times with locks and unlocks as well as wait and signal
 6. complete program
  *\*note: control C, ^C does work to exit program at anytime*
  
#### Errors
![graph displaying success rate vs number of items]( "Success rate vs. N items")  
The code functions best for smaller n and starts to fail more frequently when n is larger. I would like to know why this happens and am going to create a program to plot the success rate. 

The process to observe the error rate consisted of a C file that would perform the same actions as the actual assignment but the return value was a `boolean` flag that specified whether the operation was successful. `true`==success, it follows, `false`==fail. However the causes of the errors were not tracked. When running from the terminal line, the behavior appeared to be different. This is not a measured observation however. 
When `nitems` was 1, the program never failed. This indicates that the issue could be in the semaphore or mutex, not locking data correctly. This would also not explain why the success rate appeared relatively constant. 
