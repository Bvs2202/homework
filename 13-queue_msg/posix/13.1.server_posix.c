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
	struct mq_attr attr = {0};
	char buffer[128] = {0};
	char msg[] = "Hi!";

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 128;
	attr.mq_curmsgs = 0;

	mq1 = mq_open(queue_name1, O_CREAT | O_RDWR, 0666, &attr);
	if (mq1 == -1)
	{
		perror("mq_open");
		exit(1);
	}

	mq_send(mq1, msg, strlen(msg), 1);

	printf("Сервер: сообщение отправлено\n");

	mq2 = mq_open(queue_name2, O_CREAT | O_RDWR, 0666, &attr);
	if (mq2 == -1)
	{
		perror("mq_open");
		exit(1);
	}

	mq_receive(mq2, buffer, sizeof(buffer), NULL);

	printf("Сервер: получено сообщение: %s\n", buffer);

	mq_close(mq1);
	mq_close(mq2);

	mq_unlink(queue_name1);
	mq_unlink(queue_name2);

	return 0;
}