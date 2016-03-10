#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <cstring>
#include <cstdio>
#include <cassert>

#include "simframe.h"


class PriorityQueue
{
public:
	static const int CAPACITY = 4e8;

	PriorityQueue();
	~PriorityQueue();

	SimFrame* push(SimFrame* item);
	SimFrame* getBest();
	void reprioritize();

	bool isEmpty();

private:
	SimFrame** heap;
	int count;

	static bool ABetterThanB(const SimFrame* a, const SimFrame* b);
	static void sort(SimFrame **arr, int lo, int hi);
	static const SimFrame* median(const SimFrame* a, const SimFrame* b, const SimFrame* c);
};

#endif // PRIORITYQUEUE_H
