#include "priorityqueue.h"


PriorityQueue::PriorityQueue(int capacity)
	: capacity(capacity), count(0)
{
	heap = new SimFrame*[capacity+1];
}

PriorityQueue::~PriorityQueue()
{
	for(int i=1; i<=count; i++){
		SimFrame::freeLeaves( heap[i] );
	}
	delete heap;
}

void PriorityQueue::push(SimFrame *item)
{
	// resize as necessary
	if(capacity < count+1)
		resizeHeap(capacity*2);

	// add new item to heap
	heap[count+1] = item;
	count++;

	// bubble up as necessary
	int i = count;
	while(i>1){
		if( heap[i]->score < heap[i/2]->score ){
			auto temp = heap[i/2];
			heap[i/2] = heap[i];
			heap[i] = temp;
			i = i/2;
		}
		else
			break;
	}

}

SimFrame* PriorityQueue::pop()
{
	if(count == 0)
		return nullptr;

	// get node with highest priority
	SimFrame* head = heap[1];
	heap[1] = heap[count];
	count--;

	// bubble new head down
	int i = 1;
	while(i <= count/2)
	{
		// find more favorable child
		int betterChild;
		if( heap[2*i]->score < heap[2*i+1]->score )
			betterChild = 2*i;
		else
			betterChild = 2*i+1;

		// swap if necessary
		if(heap[i]->score > heap[betterChild]->score){
			auto temp = heap[i];
			heap[i] = heap[betterChild];
			heap[betterChild] = temp;
			i = betterChild;
		}
		else
			break;
	}

	// return item
	return head;
}

void PriorityQueue::resizeHeap(int newCapacity)
{
	SimFrame** newHeap = new SimFrame*[newCapacity+1];
	memcpy(newHeap+1, heap+1, count*sizeof(SimFrame*));
	capacity = newCapacity;
	delete heap;
	heap = newHeap;
}

bool PriorityQueue::isEmpty()
{
	return count == 0;
}
