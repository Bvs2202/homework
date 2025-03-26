#include <stdio.h>

#include "oper.h"

int main()
{
	int num1 = 0;
	int num2 = 0;
	int operation = 0;

	while (1)
	{
		printf("1) Сложение\n2) Вычитание\n3) Умножение\n4) Деление\n5) Выход\n");
		printf("Введите номер операции: ");
		if (scanf("%d", &operation) != 1)
		{
			printf("Неверная операция.\n\n");
			continue;
		}

		if (operation == 5)
		{
			return 0;
		}

		printf("Введите два числа: ");
		if (scanf("%d %d", &num1, &num2) != 2)
		{
			printf("Неверные числа.\n\n");
			continue;
		}
		
		switch (operation)
		{
			case 1:
				printf("\nРезультат: %d\n\n", add(num1, num2));
				break;
			case 2:
				printf("\nРезультат: %d\n\n", sub(num1, num2));
				break;
			case 3:
				printf("\nРезультат: %d\n\n", mul(num1, num2));
				break;
			case 4:
				if (num2 == 0)
				{
					printf("\nНевозможно разделить на ноль.\n\n");
					break;
				}
				printf("\nРезультат: %d\n\n", div(num1, num2));
				break;
			default:
				printf("Неверная операция.\n\n");
				break;
		}
	}
}