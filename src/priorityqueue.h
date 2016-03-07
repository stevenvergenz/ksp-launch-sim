#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <cstring>
#include "simframe.h"

struct PQueueItem
{
	SimFrame* item;
	double score;
};


class PriorityQueue
{
public:
	PriorityQueue(int capacity = 10000);
	~PriorityQueue();

	void push(SimFrame* item, double score);
	SimFrame* pop(double* score = nullptr);

	bool isEmpty();

private:
	PQueueItem* heap;
	int capacity;
	int count;

	void resizeHeap(int newCapacity);
};

#endif // PRIORITYQUEUE_H
