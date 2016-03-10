#include "priorityqueue.h"


PriorityQueue::PriorityQueue()
	: count(0), heap(nullptr)
{
	heap = new SimFrame*[CAPACITY+1];
}

PriorityQueue::~PriorityQueue()
{
	for(int i=1; i<=count; i++){
		delete heap[i];
	}
	delete[] heap;
}

SimFrame* PriorityQueue::push(SimFrame *item)
{
	// drop as necessary
	SimFrame* dump = nullptr;
	if(CAPACITY < count+1){
		dump = heap[count];
		heap[count] = item;
	}
	else
		heap[++count] = item;

	//printf("Heap usage: %d of %d\n", count, CAPACITY);

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
	else
		return heap[1];
}

bool PriorityQueue::ABetterThanB(const SimFrame *a, const SimFrame *b)
{
	bool aHasUnbufferedSuccessors = false;
	int i;
	for(i=0; i<a->nextCount/8; i++){
		if(a->nextBuffered[i] < 0xff){
			aHasUnbufferedSuccessors = true;
			break;
		}
	}
	if(!aHasUnbufferedSuccessors)
		aHasUnbufferedSuccessors = a->nextBuffered[i] ^ ((1 << (a->nextCount%8)) - 1);

	bool scoreRanking = a->score < b->score;
	bool timeRanking = a->score == b->score && a->time > b->time;
	bool successorsRanking = a->score == b->score && a->time == b->time && aHasUnbufferedSuccessors;

	// rank by score
	return (a->score < b->score)
		// then by depth
		|| (a->score == b->score && a->time > b->time)
		// then by viable successor count
		|| (a->score == b->score && a->time == b->time && aHasUnbufferedSuccessors);
}

// just sort the array, results in a new heap
void PriorityQueue::reprioritize()
{
	sort(heap, 1, count+1);
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
			while( j>lo && ABetterThanB(arr[j], arr[j-1]) )
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
