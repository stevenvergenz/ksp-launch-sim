#include "priorityqueue.h"

PriorityQueue::PriorityQueue(int capacity)
{
	heap = new PQueueItem[capacity+1];
	capacity = capacity;
}

PriorityQueue::~PriorityQueue()
{
	delete heap;
}

void PriorityQueue::push(SimFrame *item, double score)
{
	// resize as necessary
	if(capacity < count+1)
		resizeHeap(capacity*2);

	// add new item to heap
	heap[count].item = item;
	heap[count].score = score;
	count++;

	// bubble up as necessary
	int i = count-1;
	while(i>1){
		if( heap[i].score < heap[i/2].score ){
			auto temp = heap[i/2];
			heap[i/2] = heap[i];
			heap[i] = temp;
			i = i/2;
		}
		else
			break;
	}

}

SimFrame* PriorityQueue::pop(double* score)
{
	if(count == 0)
		return nullptr;

	// get node with highest priority
	PQueueItem head = heap[1];
	heap[1] = heap[count-1];
	count--;

	// bubble new head down
	int i = 1;
	while(i <= count/2)
	{
		// find more favorable child
		int betterChild;
		if( heap[2*i].score < heap[2*i+1].score )
			betterChild = 2*i;
		else
			betterChild = 2*i+1;

		// swap if necessary
		if(heap[i].score > heap[betterChild].score){
			auto temp = heap[i];
			heap[i] = heap[betterChild];
			heap[betterChild] = temp;
			i = betterChild;
		}
		else
			break;
	}

	// return item
	if(score != nullptr)
		*score = head.score;
	return head.item;
}

void PriorityQueue::resizeHeap(int newCapacity)
{
	PQueueItem* newHeap = new PQueueItem[newCapacity+1];
	memcpy(newHeap+1, heap+1, count*sizeof(PQueueItem));

	delete heap;
	heap = newHeap;
}
