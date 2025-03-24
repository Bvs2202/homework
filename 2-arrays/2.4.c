#include <stdio.h>
#include <unistd.h>

#define SIZE_ARR 100

int main()
{
	int arr[SIZE_ARR][SIZE_ARR] = {0};
	int num = 0;
	int count = 1;
	int j = 0, i = 0;
	int layer = 0;

	scanf("%d", &num);
	if (num <= 0)
	{
		return 0;
	}

	while(count <= num * num)
	{
		while(j < num - layer)
		{
			arr[i][j] = count++;
			j++;
		}
		i++;
		j--;

		while(i < num - layer)
		{
			arr[i][j] = count++;
			i++;
		}
		j--;
		i--;

		while(j >= layer)
		{
			arr[i][j] = count++;
			j--;
		}
		i--;
		j++;

		layer++;

		while(i >= layer)
		{
			arr[i][j] = count++;
			i--;
		}
		j++;
		i++;
	}

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}

	return 0;
}