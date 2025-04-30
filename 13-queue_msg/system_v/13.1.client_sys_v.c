#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>

struct msgbuf {
	long mtype;
	char mtext[100];
};

int main()
{
	key_t key;
	int msgid = 0;
	struct msgbuf msg = {0};

	key = ftok("13.1.server_sys_v.c", 1);
	if (key == -1)
	{
		perror("ftok");
		exit(1);
	}

	msgid = msgget(key, 0666);
	if (msgid == -1)
	{
		perror("msgget");
		exit(1);
	}

	msgrcv(msgid, &msg, sizeof(msg.mtext), 2, 0);

	printf("Клиент: получено сообщение: %s\n", msg.mtext);

	msg.mtype = 1;
	strncpy(msg.mtext, "Hello!", sizeof(msg.mtext) - 1);
	msg.mtext[sizeof(msg.mtext) - 1] = '\0';
	msgsnd(msgid, &msg, sizeof(msg.mtext), 0);

	printf("Клиент: сообщение отправлено\n");

	return 0;
}