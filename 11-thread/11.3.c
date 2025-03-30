#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define COUNT_CONSUMER 3
#define COUNT_PRODUCER 1

#define TARGET_CONSUMER 10000

#define PRODUECT_QUANTITY 500

#define NUMBER_ROOM_SHOP 5

pthread_mutex_t mutex[NUMBER_ROOM_SHOP] = {PTHREAD_MUTEX_INITIALIZER};
pthread_mutex_t end_mutex = PTHREAD_MUTEX_INITIALIZER;

int shop[NUMBER_ROOM_SHOP] = {500, 500, 500, 500, 500};
int flag_end = 0;

void *consumer_func(void *arg)
{
	int *index = (int *)arg;
	int bag = 0;
	int remainder = 0;

	while (1)
	{
		for (int i = 0; i < NUMBER_ROOM_SHOP; i++)
		{
			if (pthread_mutex_trylock(&mutex[i]) == 0)
			{
				if (shop[i] > 0)
				{
					if (bag + shop[i] >= TARGET_CONSUMER)
					{
						remainder = bag + shop[i] - TARGET_CONSUMER;
						bag = bag + (shop[i] - remainder);
						printf("Покупатель %d купил %d товаров в %d комнате, осталось купить %d\n", *index, shop[i] - remainder, i + 1, TARGET_CONSUMER - bag);
						shop[i] = remainder;
						goto end;
					}
					else
					{
						bag = bag + shop[i];
						printf("Покупатель %d купил %d товаров в %d комнате, осталось %d\n", *index, shop[i], i + 1, TARGET_CONSUMER - bag);
						shop[i] = 0;
					}
				}
				pthread_mutex_unlock(&mutex[i]);
				sleep(2);
			}
		}
	}		
	end:

	printf("\nПокупатель %d купил необходимое количество товаров\n\n", *index);

	pthread_mutex_lock(&end_mutex);
	flag_end++;
	pthread_mutex_unlock(&end_mutex);

	pthread_exit(NULL);
}

void *producer_func(void *arg)
{
	while (1)
	{
		for (int i = 0; i < NUMBER_ROOM_SHOP; i++)
		{
			if (flag_end == COUNT_CONSUMER)
			{
				pthread_exit(NULL);
			}
			if (pthread_mutex_trylock(&mutex[i]) == 0)
			{
				shop[i] = shop[i] + PRODUECT_QUANTITY;
				printf("Производитель добавил %d товаров в %d комнату\n", PRODUECT_QUANTITY, i + 1);

				pthread_mutex_unlock(&mutex[i]);
				sleep(1);
			}
		}
	}
}

int main()
{
	pthread_t consumer[COUNT_CONSUMER];
	pthread_t producer[COUNT_PRODUCER];
	int index[COUNT_CONSUMER] = {0};

	for (int i = 0; i < COUNT_CONSUMER; i++)
	{
		index[i] = i + 1;
		pthread_create(&consumer[i], NULL, consumer_func, (void *)&index[i]);
	}

	for (int i = 0; i < COUNT_PRODUCER; i++)
	{
		pthread_create(&producer[i], NULL, producer_func, NULL);
	}

	for (int i = 0; i < COUNT_CONSUMER; i++)
	{
		pthread_join(consumer[i], NULL);
	}

	pthread_join(producer[0], NULL);

	return 0;
}