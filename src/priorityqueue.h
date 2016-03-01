#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "simframe.h"

struct PQueueItem
{
	SimFrame* item;
	PQueueItem* prev;
	PQueueItem* next;
};


class PriorityQueue
{
public:
	PriorityQueue();
	~PriorityQueue();

	void insert(SimFrame* item, double score);
	void pop();

signals:

public slots:

private:
	PQueueItem* head;

};

#endif // PRIORITYQUEUE_H
