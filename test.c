#include <stdlib.h>
#include <stdio.h>

int ABetterThanB(int a, int b)
{
	return a < b;
}

const int median(const int a, const int b, const int c)
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


void sort(int* arr, int lo, int hi)
{
	// base case (array length 0 or 1)
	if(hi-lo < 2){
		//printf("%d to %d already sorted\n", lo, hi);
		return;
	}

	// do an old-fashioned insertion sort for small arrays
	else if(hi-lo < 9)
	{
		//printf("ins sort from %d to %d\n", lo, hi);
		for(int i=lo+1; i<hi; i++)
		{
			int j=i;
			while( ABetterThanB(arr[j], arr[j-1]) && j>lo )
			{
				int temp = arr[j-1];
				arr[j-1] = arr[j];
				arr[j] = temp;
				j--;
			}
		}

		/*for(int i=0; i<20; i++)
			printf("%2d ", arr[i]);
		printf("\n");*/
	}

	// quick sort, using median-of-nines partitioning
	else
	{
		// [0-9) -> 0,1,2, 3,4,5, 6,7,8
		// [0-100) -> 0,16,32, 33,49,65, 66,82,99
		int p1 = lo+(hi-lo)/3, p2 = lo+2*(hi-lo)/3;
		const int pivot = median(
			median(arr[lo], arr[lo+(p1-lo)/2], arr[p1-1]),
			median(arr[p1], arr[p1+(p2-p1)/2], arr[p2-1]),
			median(arr[p2], arr[p2+(hi-p2)/2], arr[hi-1])
		);

		//printf("pivot from %d to %d: %d\n", lo, hi, pivot);

		int i = lo;
		for(int j=lo; j<hi; j++)
		{
			if( ABetterThanB(arr[j], pivot) ){
				int temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
				i++;
			}
		}

		/*for(int i=0; i<20; i++)
			printf("%2d ", arr[i]);
		printf("\n");*/

		sort(arr, lo, i);
		sort(arr, i, hi);
	}

}

int main()
{
	const int LENGTH = 50;
	int arr[LENGTH];

	int count, i, j;
	for(count=0; count<10; count++)
	{
		for(i=0; i<LENGTH; i++)
			arr[i] = rand() % 100;

		for(i=0; i<LENGTH; i++)
			printf("%2d ", arr[i]);
		printf("\n");

		sort(arr, 0, LENGTH);

		for(i=0; i<LENGTH; i++){
			printf("%2d ", arr[i]);
			if(i>0 && arr[i] < arr[i-1])
				printf("INVALID-----------------------\n");
		}
		printf("\n\n");
	}

	return 0;
}

