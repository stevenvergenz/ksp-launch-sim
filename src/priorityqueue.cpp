#include "priorityqueue.h"


PriorityQueue::PriorityQueue(int capacity)
	: capacity(capacity), count(0)
{
	heap = new SimFrame*[capacity+1];
}

PriorityQueue::~PriorityQueue()
{
	for(int i=1; i<=count; i++){
		delete heap[i];
	}
	delete heap;
}

SimFrame* PriorityQueue::push(SimFrame *item)
{
	// drop as necessary
	SimFrame* dump = nullptr;
	if(capacity < count+1){
		dump = heap[count];
		heap[count] = item;
	}
	else
		heap[++count] = item;

	// bubble up as necessary
	int i = count;
	while(i>1){
		if( ABetterThanB(heap[i], heap[i/2]) ){
			auto temp = heap[i/2];
			heap[i/2] = heap[i];
			heap[i] = temp;
			i = i/2;
		}
		else
			break;
	}

	return dump;
}

SimFrame* PriorityQueue::getBest()
{
	if(count == 0)
		return nullptr;
	else return heap[1];
}

bool PriorityQueue::ABetterThanB(const SimFrame *a, const SimFrame *b)
{
	if(a->score < b->score)
		return true;
	else if(a->score > b->score)
		return false;
	else if(a->time > b->time)
		return true;
	else if(a->time < b->time)
		return false;
	else
		return true; // a has at least one unbuffered successor
}

// just sort the array, results in a new heap
void PriorityQueue::reprioritize()
{
	quickSort(heap, 1, count+1);
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

// sort arr from [lo,hi)
void PriorityQueue::sort(SimFrame** arr, int lo, int hi)
{
	// base case (array length 0 or 1)
	if(hi-lo < 2){
		return;
	}

	// do an old-fashioned insertion sort for small arrays
	else if(hi-lo < 9)
	{
		for(int i=lo+1; i<hi; i++)
		{
			int j=i;
			while( ABetterThanB(arr[j], arr[j-1]) && j>lo )
			{
				auto temp = arr[j-1];
				arr[j-1] = arr[j];
				arr[j] = temp;
				j--;
			}
		}
	}

	// quick sort, using median-of-nines partitioning
	else
	{
		// [0-9) -> 0,1,2, 3,4,5, 6,7,8
		// [0-100) -> 0,16,32, 33,49,65, 66,82,99
		int p1 = lo+(hi-lo)/3, p2 = lo+2*(hi-lo)/3;
		const SimFrame* pivot = median(
			median(arr[lo], arr[lo+(p1-lo)/2], arr[p1-1]),
			median(arr[p1], arr[p1+(p2-p1)/2], arr[p2-1]),
			median(arr[p2], arr[p2+(hi-p2)/2], arr[hi-1])
		);

		int i = lo;
		for(int j=lo; j<hi; j++)
		{
			if( ABetterThanB(arr[j], pivot) ){
				auto temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
				i++;
			}
		}

		sort(arr, lo, i);
		return sort(arr, i, hi);
	}
}

const SimFrame* PriorityQueue::median(const SimFrame *a, const SimFrame *b, const SimFrame *c)
{
	if(ABetterThanB(a, b))
		if(ABetterThanB(b, c)) // a b c
			return b;
		else if(ABetterThanB(a, c)) // a c b
			return c;
		else //	c a b
			return a;
	else
		if(ABetterThanB(a, c)) // b a c
			return a;
		else if(ABetterThanB(b, c)) // b c a
			return c;
		else // c b a
			return b;
}
