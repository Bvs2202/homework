#include <stdio.h>

#define SIZE_ARR 3

int main()
{
	for (int i = 0; i < SIZE_ARR; i++)
	{
		for (int j = 0; j < SIZE_ARR; j++)
		{
			if (i + j >= SIZE_ARR - 1)
			{
				printf("1 ");
			}
			else
			{
				printf("0 ");
			}
		}
		printf("\n");
	}

	return 0;
}