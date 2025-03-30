#include <stdio.h>

#define TARGET 180000000

int main()
{
    int num = 0;

    while (num != TARGET)
    {
        num++;
    }

    printf("num = %d\n", num);

    return 0;
}