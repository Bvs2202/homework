#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define COUNT_THREAD 6
#define TARGET 180000000

void *thread_func(void *arg)
{
    int *local_num;

    local_num = malloc(sizeof(int));
    if (local_num == NULL)
    {
        return NULL;
    }

    *local_num = 0;

    for (int i = 0; i < TARGET / COUNT_THREAD; i++)
    {
        (*local_num)++;
    }

    return local_num;
}

int main()
{
    pthread_t thread[COUNT_THREAD];
    int res = 0;
    int *num;

    for (int i = 0; i < COUNT_THREAD; i++)
    {
        pthread_create(&thread[i], NULL, thread_func, NULL);
    }

    for (int i = 0; i < COUNT_THREAD; i++)
    {
        pthread_join(thread[i], (void **)&num);
        res += *num;
        free(num);
    }

    printf("num = %d\n", res);

    return 0;
}