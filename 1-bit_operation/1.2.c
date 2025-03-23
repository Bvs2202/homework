#include <stdio.h>

int main()
{
	int num = 0;
	int bit = 0;
	int space = 0;

	scanf("%d", &num);

	for (int i = 31; i >= 0; i--)
	{
		if (space == 4)
		{
			printf(" ");
			space = 0;
		}
		bit = ((num >> i) & 1);
		printf("%d", bit);
		space++;
	}
	printf("\n");

	return 0;
}