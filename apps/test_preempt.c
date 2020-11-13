#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
#include <time.h>


void thread3(void *arg)
{
        clock_t start = clock();
        double cpu_time_used = 0;

        for (int i = 0; i < 5; ++i) {
                while (cpu_time_used < 0.0095) {
                        cpu_time_used = ((double) (clock() - start)) / CLOCKS_PER_SEC;
                }
                printf("thread 3\n");
                start = clock();
                cpu_time_used = 0;
        }
}

void thread2(void *arg)
{
        clock_t start = clock();
        double cpu_time_used = 0;

        for (int i = 0; i < 5; ++i) {
                while (cpu_time_used < 0.0095) {
                        cpu_time_used = ((double) (clock() - start)) / CLOCKS_PER_SEC;
                }
                printf("thread 2\n");
                start = clock();
                cpu_time_used = 0;
        }
}

void thread1(void *arg)
{
	uthread_create(thread2, NULL);
        uthread_create(thread3, NULL);

	clock_t start = clock();
        double cpu_time_used = 0;

        for (int i = 0; i < 5; ++i) {
                while (cpu_time_used < 0.0095) {
                        cpu_time_used = ((double) (clock() - start)) / CLOCKS_PER_SEC;
                }
                printf("thread 1\n");
                start = clock();
                cpu_time_used = 0;
        }

}

int main(void)
{
	uthread_start(thread1, NULL);
	return 0;
}
