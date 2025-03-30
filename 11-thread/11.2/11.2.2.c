#include <stdio.h>
#include <pthread.h>

#define COUNT_THREAD 6
#define TARGET 180000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int num = 0;

void *thread_func(void *arg)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < TARGET / COUNT_THREAD; i++)
    {
        num++;
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    pthread_t thread[COUNT_THREAD];

    for (int i = 0; i < COUNT_THREAD; i++)
    {
        pthread_create(&thread[i], NULL, thread_func, NULL);
    }

    for (int i = 0; i < COUNT_THREAD; i++)
    {
        pthread_join(thread[i], NULL);
    }

    printf("num = %d\n", num);

    return 0;
}