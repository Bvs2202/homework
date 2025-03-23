#include <stdio.h>

int main()
{
	int num = 0;
	int bit = 0;
	int space = 0;
	char swap = 0;

	printf("Введите целое число: ");
	scanf("%d", &num);
	printf("Введите число(0-255): ");
	scanf("%hhd", &swap);

	printf("Исходное число: ");
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

	num = ((0xFF00FFFF & num) | (swap << 16));

	space = 0;

	printf("Измененное число: ");
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