Queue:

For this part of the project, the idea is to use a structure that contains the head of the queue and the tail of the queue. The middle of the queue is connected by a single linked list. We choose this structure is because the dequeue from the head and append to the end will be done in O(1) time, and it is memory friendly that it only expand the linked list big enough for the data. With this data structure, the queue and dequeue process can be done by only modify the head and tail, and then connect the new one to the linked list.

For testing, we added unit tests for each function to test the return value and to check if delete, iterate and others work properly. Then, we had an intergration test which is important in testing to see how different functions work together. We have some corner cases such as enqueueing an element and then deleted immdiately, adding and dequeueing one element to see if the pointer is freed and equal to NULL, dequeueing each element to see if in order, and several random queueing and dequeueing.


Uthread:

We had 3 queues as static global variable which are ready-queue, blocked-queue and zombie-queue to store the TCBs for each process. And one TCB struct is created as a static global variable that contains the information of the current running thread. The reason we choose this design is because global queue can be shared by all threads since they all share the same address space from main() despite their function stack is allocated differentlly in the heap as virtual stack. We also make them static so that it is private variables can't be accessed from the outside of the file.

We then looked at the code in context.c to understand how swap context and makecontext work, and we only change context when yield, exit and block is called because for this cooperative part, these are the only exits for on thread to enter another. When ever swithcing context is called, the current thread's state is changed and equeued into the corresponding group, and dequeue the next ready thread. The job for the thread_start function is to create the first thread and privde an exit to the main function, having a while loop to keep yielding until the ready queue and block queue is NULL. Then we can free the queues and go back to main.


Sem:

Inside the sem data structure, our design is to have a count to count the resources and a queue to save waiting threads when the resource is no avaliable, and then we can wake them up later by dequeue. This section is pretty straight foward as discussed during lectures. The only change added to Sem_down is to prevent over taking resources and starvation. For prevent overtaking when awaken, we have an while loop that keep checking the resources so if not avialiable, it gets blocked again. For preventing starvation, we had a count inside the while loop to check how many times it got blocked when trying to grap resource. The possibilityis that they might not be the first one to be unblocked or the ready queue has some others ready to grap as well. To prevent this, when the count reaches 5, we no longer put that in block but have it yield instead as busy waiting to grab the resource as soon as possible.


Preempt:

The major time spent in this part is to understand how setitimer works and how signal is handled. After reading the GNU file, when trying that when setting up setitimer, if the time interval in microsecond is greater or equal to 1000000 which is one second, it will not work so that we write a little algorithm to write the second part and microsecond part seperatly. We convert frequency in microseconds by dividing 1000000 by the frequency in HZ. The only job for the signal handler is to call yield() so that another thread will be switched into. The preempt enable, disable and stop is also really straight forward that we just have to unblock the signal in enable, block it in disable and restore the clock and signal in stop.

For the testing part, we firstly tried one mentioned in piazza to have one thread loop forever and have another one call exit() with no yield() called inbetween. It works but not ideal to show the round-robin algorithm. In order to show that, we use a function to wait for nearly .01 seconds to print out some results. Each thread will take turns to print out their results until they ended. The problem is that sleep is not working because it doens't take CPU time, so we use clock() to get the current CPU time and find out the time elapsed divided by CLOCKS_PER_SEC. A while loop will kill times in CPU and wait until the time reacheds 0.01 second.

For critical sections, in thread.c, it is needed whenever it's reading or writing in the static global queues and while perform context switch which modifies CURRENT_THREAD global variable. In thread_create, a critical section is only needed when enqueueing the ready thread. For semaphores, a criticial section is need for grabing a resource and releasing a resource.
