#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define COUNT_THREAD 5

void *thread_func(void *arg)
{
	int *index = (int *)arg;

	printf("%d\n", *index);
	return NULL;
}

int main()
{
	pthread_t thread[COUNT_THREAD];
	int index[COUNT_THREAD] = {0};

	for (int i = 0; i < COUNT_THREAD; i++)
	{
		index[i] = i;
		pthread_create(&thread[i], NULL, thread_func, (void *)&index[i]);
	}

	for (int i = 0; i < COUNT_THREAD; i++)
	{
		pthread_join(thread[i], NULL);
	}

	return 0;
}
