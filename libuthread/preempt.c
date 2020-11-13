#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

static struct itimerval old, new;
static struct sigaction sa;
static sigset_t block_alarm;

//Referenced and modified from GNU.org
unsigned int u_alarm (unsigned int frequency)
{
	long int sec;
    	long int usec;

    	//alarm in 1 socond.
    	if (frequency == 1) {
        	sec = 1;
        	usec = 0;
    	} else { //alarm in less than 1 second.
        	sec = 0;
        	usec = 1000000 / frequency;
    	}

    	new.it_interval.tv_usec = usec;
    	new.it_interval.tv_sec = sec;
    	new.it_value.tv_usec = usec;
    	new.it_value.tv_sec = sec;
    	if (setitimer(ITIMER_VIRTUAL, &new, &old) < 0)
        	return 0;
    	else
        	return old.it_value.tv_sec;
}

unsigned int remove_alarm(void)
{
	//Set the timer off.
	new.it_interval.tv_usec = 0;
    	new.it_interval.tv_sec = 0;
    	new.it_value.tv_usec = 0;
    	new.it_value.tv_sec = 0;
    	if (setitimer (ITIMER_VIRTUAL, &new, &old) < 0)
        	return 0;
    	else
        	return old.it_value.tv_sec;
}

//handle the alarm to interrupt a thread and switch to another thread if any.
void uthread_preemption_handler(__attribute__((unused)) int signum)
{
	uthread_yield();
}

void preempt_disable(void)
{
	// Ignore alarm.
	sigemptyset(&block_alarm);
	sigaddset(&block_alarm, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &block_alarm, NULL);
}

void preempt_enable(void)
{
	//restore alarm.
	sigemptyset(&block_alarm);
    	sigaddset(&block_alarm, SIGVTALRM);
    	sigprocmask(SIG_UNBLOCK, &block_alarm, NULL);
}

void preempt_start(void)
{
	//Set up uthread_preemption_handler for alarm
    	sa.sa_handler = uthread_preemption_handler;
    	sigemptyset(&sa.sa_mask);
    	sa.sa_flags = 0;
    	sigaction(SIGVTALRM, &sa, NULL);

	//Configure the timer based on the frequency.
	u_alarm(HZ);


}

void preempt_stop(void)
{
	//remove the alarm.
	remove_alarm();

	//Set up back to default handler
    	sa.sa_handler = SIG_DFL;
    	sigaction(SIGVTALRM, &sa, NULL);

}
