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
	char buffer[100] = "Hi!";

	key = ftok("13.1.server_sys_v.c", 1);
	if (key == -1)
	{
		perror("ftok");
		exit(1);
	}

	msgid = msgget(key, 0666 | IPC_CREAT);
	if (msgid == -1)
	{
		perror("msgget");
		exit(1);
	}

	msg.mtype = 2;
	strncpy(msg.mtext, buffer, sizeof(msg.mtext) - 1);
	msg.mtext[sizeof(msg.mtext) - 1] = '\0';

	if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1)
	{
		perror("msgsnd");
		exit(1);
	}

	printf("Сервер: сообщение отправлено\n");

	msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);

	printf("Сервер: получено сообщение: %s\n", msg.mtext);

	msgctl(msgid, IPC_RMID, NULL);

	return 0;
}