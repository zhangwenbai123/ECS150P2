#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

static queue_t Ready_Queue;
static queue_t Blocked_Queue;
static queue_t Zombie_Queue;
static struct uthread_tcb *Current_Thread;

typedef enum {
	Ready,
	Running,
	Blocked,
	Zombie
} threadState;

struct uthread_tcb {
	uthread_ctx_t thread_context;
	void *stack_top;
	threadState thread_state;
};

struct uthread_tcb *uthread_current(void)
{
	return Current_Thread;
}

void uthread_yield(void)
{
	int retval;
	struct uthread_tcb *current;
	struct uthread_tcb *next_thread;

	//Critical section.....................................................
	preempt_disable();

	//get the next ready thread out of the ready queue.
	retval = queue_dequeue(Ready_Queue, (void**)(&next_thread));
	if (retval == -1) { //No ready queue.
		return;
	}

	next_thread->thread_state = Running;

	//Put current thread in ready queue and switch to the next.
	Current_Thread->thread_state = Ready;
	queue_enqueue(Ready_Queue, Current_Thread);
	current = Current_Thread;
	Current_Thread = next_thread;
	uthread_ctx_switch(&(current->thread_context), &(next_thread->thread_context));

	preempt_enable();
	//Exit Critical section................................................

}

void uthread_exit(void)
{
	struct uthread_tcb *current;
	struct uthread_tcb *next_thread;

	//Critical section.....................................................
	preempt_disable();

	//get the next ready thread out of the ready queue.
	queue_dequeue(Ready_Queue, (void**)(&next_thread));
	next_thread->thread_state = Running;

	//Put current thread in zombie queue and switch to the next.
	Current_Thread->thread_state = Zombie;
	queue_enqueue(Zombie_Queue, Current_Thread);

	current = Current_Thread;
	Current_Thread = next_thread;
	uthread_ctx_switch(&(current->thread_context), &(next_thread->thread_context));

	preempt_enable();
	//Exit Critical section................................................
}

int uthread_create(uthread_func_t func, void *arg)
{
	int retval;

	//initialize the TCB for the created thread.
	struct uthread_tcb *created_thread = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if (created_thread == NULL)
		return -1;
	created_thread->stack_top = uthread_ctx_alloc_stack();
	retval = uthread_ctx_init(&(created_thread->thread_context),
				    created_thread->stack_top, func, arg);
	if (retval == -1)
		return -1;

	//Push in into the ready queue for the next time to be executed.
	created_thread->thread_state = Ready;

	//Critical section.....................................................
	preempt_disable();
	queue_enqueue(Ready_Queue, created_thread);
	preempt_enable();
	//Exit Critical section................................................

	return 0;
}

void collectZombieQueue()
{
	//It doesn't need to be in a critical section
	//because it's called after threads finished for this project particully.
	struct uthread_tcb *ptr;
	while (queue_dequeue(Zombie_Queue, (void**)(&ptr)) == 0) {
		uthread_ctx_destroy_stack(ptr->stack_top);
		free(ptr);
	}

	queue_destroy(Zombie_Queue);
	Zombie_Queue = NULL;

}

int uthread_start(uthread_func_t func, void *arg)
{
	Ready_Queue = queue_create();
	Blocked_Queue = queue_create();
	Zombie_Queue = queue_create();

	//Save current running program as the idle thread.
	struct uthread_tcb *this_tcb = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	if (this_tcb == NULL)
		return -1;
	this_tcb->stack_top = NULL;   //This function shares same stack as main().
	this_tcb->thread_state = Running;
	Current_Thread = this_tcb;

	//start the preemption.
	preempt_start();


	uthread_create(func, arg);

	while (1) {
		//If there is no more thread to be perfermed, collect zombies and return to main function.
		if (queue_length(Ready_Queue) == 0 && queue_length(Blocked_Queue) == 0) {
			preempt_stop(); //stop the preemption.

			queue_destroy(Ready_Queue);
			queue_destroy(Blocked_Queue);
			Ready_Queue = NULL;
			Blocked_Queue = NULL;
			collectZombieQueue();
			return 0;
		}
		uthread_yield();
	}

}


void uthread_block(void)
{
	struct uthread_tcb *current;
	struct uthread_tcb *next_thread;

	//get the next ready thread out of the ready queue.

	//Critical section.....................................................
	preempt_disable();

	queue_dequeue(Ready_Queue, (void**)(&next_thread));
	next_thread->thread_state = Running;

	//Put current thread in block queue and switch to the next.
	Current_Thread->thread_state = Blocked;
	queue_enqueue(Blocked_Queue, Current_Thread);
	current = Current_Thread;
	Current_Thread = next_thread;
	uthread_ctx_switch(&(current->thread_context), &(next_thread->thread_context));

	preempt_enable();
	//Exit Critical section................................................
}


void uthread_unblock(struct uthread_tcb *uthread)
{
	//Critical section.....................................................
	preempt_disable();

	//If the data was not found, do nothing.
	if (queue_delete(Blocked_Queue, uthread)){
		return;
	}

	uthread->thread_state = Ready;
	queue_enqueue(Ready_Queue, uthread);

	preempt_enable();
	//Exit Critical section................................................
}
