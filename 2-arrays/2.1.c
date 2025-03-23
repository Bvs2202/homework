#include <stdio.h>

int main()
{
	int num = 0;
	int count = 1;

	scanf("%d", &num);

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < num; j++)
		{
			printf("%d ", count);
			count++;
		}
		printf("\n");
	}

	return 0;
}