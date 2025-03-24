#include <stdio.h>

int main()
{
	int num = 0;
	char swap = 0;
	char *pointer;

	scanf("%d", &num);
	scanf("%hhd", &swap);

	pointer = &num;

	printf("%x\n", num);
	pointer[2] = swap;
	printf("%x\n", num);

	return 0;
}