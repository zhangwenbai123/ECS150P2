#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	int internal_count;
	queue_t waiting_list;
};

sem_t sem_create(size_t count)
{
	sem_t output = (sem_t)malloc(sizeof(struct semaphore));
	output->internal_count = count;
	output->waiting_list = queue_create();

	return output;
}

int sem_destroy(sem_t sem)
{
	//if the length is 0 or sem is NULL
	if ((!queue_length(sem->waiting_list)) || !sem) {
		return -1;
	}

	//Destroy the queue inside and free sem.
	queue_destroy(sem->waiting_list);
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	//Starvating preventing.
	//A int keeps track of how many tries for grabbing a resource.
	//If the try is larger than 5, then it will make it busy waiting.
	int try = 0;

	if (sem == NULL) {
		return -1;
	}

	//Critical section.....................................................
	preempt_disable();

	//If the count is 0, it got blocked and check again when awaken.
	while (sem->internal_count == 0) {
		if (try < 5) {
			queue_enqueue(sem->waiting_list, uthread_current());
			uthread_block();
			try += 1;
		} else {
			/*When try so many times, it will put it in ready queue
			as busy waiting. so that it has a better chance to
			grab resource to prevent starvation.*/
			uthread_yield();
		}
	}

	//grap the resource.
	sem->internal_count -= 1;

	preempt_enable();
	//Exit Critical section................................................

	return 0;
}

int sem_up(sem_t sem)
{
	struct uthread_tcb *thread_to_be_waken;

	//Critical section.....................................................
	preempt_disable();

	if (sem == NULL) {
		return -1;
	}

	sem->internal_count += 1;
	//if there is something in the blocked list to be awaken.
	if (!queue_dequeue(sem->waiting_list, (void**)&(thread_to_be_waken))){
		uthread_unblock(thread_to_be_waken);
	}

	preempt_enable();
	//Exit Critical section................................................

	return 0;
}
