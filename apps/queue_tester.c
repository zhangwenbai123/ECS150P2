#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ........................ ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)


queue_t q;

static void inc_item(void *data)
{
    	int *a = (int*)data;
    	if (*a == 5)
        	queue_delete(q, data);
   	else
        	*a += 1;
}

static void print_data(void *data)
{
    	int *a = (int*)data;
    	printf(" %d", *a);
}

void print_queue(queue_t q){
	printf("The queue is :");
	queue_iterate(q, print_data);
	printf("\n");
}





//Test cases------------------------------------------------------------

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
	fprintf(stderr, "----------\n\n");
}

void test_delete()
{
	fprintf(stderr, "*** TEST delete ***\n");

	int b = 11;
	int c = 22;
	int d = 33;
	int *ptr;
	int retval;

	retval = queue_delete(q, &d);
	TEST_ASSERT(retval == -1);

	queue_t q = queue_create();

	queue_enqueue(q, &b);
	queue_enqueue(q, &c);
	retval = queue_delete(q, &d);
	TEST_ASSERT(retval == -1);
	retval = queue_delete(q, NULL);
	TEST_ASSERT(retval == -1);
	retval = queue_delete(q, &b);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == c);
	TEST_ASSERT(queue_length(q) == 0);


	fprintf(stderr, "----------\n\n");
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
	fprintf(stderr, "----------\n\n");
}

//iterator and delete test.
void test_iterator(void)
{
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int i;

	fprintf(stderr, "*** TEST iterator ***\n");

    	/* Initialize the queue and enqueue items */
    	q = queue_create();
    	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        	queue_enqueue(q, &data[i]);


    	/* Increment every item of the queue, delete item '5' */
   	queue_iterate(q, inc_item);
    	TEST_ASSERT(data[0] == 2);
	TEST_ASSERT(data[4] == 5);
    	TEST_ASSERT(queue_length(q) == 9);

	fprintf(stderr, "----------\n\n");
}


//intergration test.
void text_intergration(void)
{
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int b = 11;
	int i;
	int *ptr;

	queue_t q = queue_create();

	fprintf(stderr, "*** TEST intergration ***\n");

	queue_enqueue(q, &data[0]);
	queue_delete(q, &data[0]);

	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &b);
	queue_dequeue(q, (void**)&ptr);

    	/* Initialize the queue and enqueue items */
    	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        	queue_enqueue(q, &data[i]);

	printf("Initially:              ");
	print_queue(q);

	queue_dequeue(q, (void**)&ptr);

	printf("After dequeue once    : ");
	print_queue(q);
	TEST_ASSERT(*ptr == 11);

	queue_enqueue(q, &data[0]);
	printf("Enqueue element 1 back: ");
	print_queue(q);


    	queue_delete(q, &data[4]);
	printf("After delete 5 element: ");
	print_queue(q);

	TEST_ASSERT(queue_length(q) == 10);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 1);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 2);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 3);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 4);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 6);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 7);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 8);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 9);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 10);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(*ptr == 1);
	TEST_ASSERT(queue_length(q) == 0);


	fprintf(stderr, "----------\n\n");
}



//Main---------------------------------------------------------------

int main(void)
{
	test_create();
	test_queue_simple();
	test_iterator();
	test_delete();
	text_intergration();

	printf("All test passed!\n");

	return 0;
}
