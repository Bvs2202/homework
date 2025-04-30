#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

int main()
{
	const char *queue_name1 = "/queue_server";
	const char *queue_name2 = "/queue_client";
	mqd_t mq1 = 0;
	mqd_t mq2 = 0;
	char buffer[128] = {0};
	char msg[] = "Hello!";

	mq1 = mq_open(queue_name1, O_RDWR, 0666, NULL);
	if (mq1 == -1)
	{
		perror("mq_open");
		exit(1);
	}

	mq_receive(mq1, buffer, sizeof(buffer), NULL);

	printf("Клиент: получено сообщение: %s\n", buffer);

	mq2 = mq_open(queue_name2, O_RDWR, 0666, NULL);
	if (mq2 == -1)
	{
		perror("mq_open");
		exit(1);
	}

	mq_send(mq2, msg, strlen(msg), 1);

	printf("Клиент: сообщение отправлено\n");

	return 0;
}