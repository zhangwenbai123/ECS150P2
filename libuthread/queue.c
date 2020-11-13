#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

typedef struct queue_item* queue_item_t;

struct queue {
	queue_item_t head;
	queue_item_t tail;
	int numItems;
};

struct queue_item {
	void *data;
	queue_item_t next;
};



queue_t queue_create(void)
{
	queue_t output = (queue_t)malloc(sizeof(struct queue));

	//Check if output malloc is sucessful.
	if (!output) {
		return NULL;
	} else {
		output->head = NULL;
		output->tail = NULL;
		output->numItems = 0;
		return output;
	}
}

int queue_destroy(queue_t queue)
{
	if ((!queue) || (queue->head != NULL)) {
		return -1;
	}

	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}

	queue_item_t newItem = (queue_item_t)malloc(sizeof(struct queue_item));
	if (!newItem) {
		return -1;
	}
	newItem->data = data;
	newItem->next = NULL;

	//if the queue is not empty and it has a tail.
	if (queue->tail) {
		queue->tail->next = newItem;
		queue->tail = newItem;
	} else {
		queue->head = newItem;
		queue->tail = newItem;
	}
	queue->numItems += 1;
	return 0;

}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->head == NULL) {
		return -1;
	}
	*data = queue->head->data;
	//If there is only one item left in the queue
	if (queue->head == queue->tail) {
		free(queue->head);
		queue->head = NULL;
		queue->tail = NULL;
	} else {
		queue_item_t nextHead = queue->head->next;
		free(queue->head);
		queue->head = nextHead;
	}
	queue->numItems -= 1;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}

	queue_item_t iterator = queue->head;
	queue_item_t previous = NULL;
	while (iterator) {
		if (data == iterator->data) {
			//If it's the head of the queue, modify the head.
			if (previous == NULL) {
				queue->head = iterator->next;
			} else {
				//make the connection from previous to next.
				previous->next = iterator->next;
			}

			//If it is the last of the queue, modify the tail.
			if (iterator->next == NULL) {
				queue->tail = previous;
			}

			free(iterator);
			queue->numItems -= 1;
			return 0;
		}
		previous = iterator;
		iterator = iterator->next;
	}

	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue == NULL || func == NULL) {
		return -1;
	}
	queue_item_t iterator = queue->head;
	while(iterator) {
		(*func)(iterator->data);
		iterator = iterator->next;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->numItems;
}
