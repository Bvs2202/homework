#include <stdio.h>

#define SIZE_ARR 10

int main()
{
	int arr[SIZE_ARR] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int *ptr;

	ptr = arr;

	for (int i = 0; i < SIZE_ARR; i++)
	{
		printf("%d ", *ptr);
		ptr++;
	}
	printf("\n");

	return 0;
}