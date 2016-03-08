#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <cstring>
#include "simframe.h"


class PriorityQueue
{
public:
	PriorityQueue(int capacity = 10000);
	~PriorityQueue();

	void push(SimFrame* item);
	SimFrame* pop();

	bool isEmpty();

private:
	SimFrame** heap;
	int capacity;
	int count;

	void resizeHeap(int newCapacity);
};

#endif // PRIORITYQUEUE_H
