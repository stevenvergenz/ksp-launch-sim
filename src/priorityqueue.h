#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <cstring>
#include "simframe.h"


class PriorityQueue
{
public:
	PriorityQueue(int capacity = 4e6);
	~PriorityQueue();

	SimFrame* push(SimFrame* item);
	SimFrame* getBest();
	void reprioritize();

	bool isEmpty();

private:
	SimFrame** heap;
	int capacity;
	int count;

	void resizeHeap(int newCapacity);
	static bool ABetterThanB(const SimFrame* a, const SimFrame* b);
	static void quickSort(SimFrame **arr, int lo, int hi);
	static const SimFrame* median(const SimFrame* a, const SimFrame* b, const SimFrame* c);
};

#endif // PRIORITYQUEUE_H
