#include <stdio.h>

#define SIZE_ARR 10

int main()
{
    int num = 0;
    int arr[SIZE_ARR] = {0};

    scanf("%d", &num);

    printf("Исходный массив: ");
    for (int i = 1; i <= num; i++)
    {
        arr[i] = i;
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("Перевернутый массив: ");
    for (int i = num; i > 0; i--)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");

	return 0;
}