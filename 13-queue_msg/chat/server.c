#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <pthread.h>

#define REGISTRATION 10
#define CONFIRM_REG 11
#define MSG_SERVER 12
#define MSG_CHAT 13
#define NEW_LIST_CLIENTS 14
#define CLIENT_EXIT 15
#define SERVER_EXIT 666

#define SIZE_TEXT 64
#define NUM_CLIENTS 16
#define CLIENT_ID 100
#define CHAT_HISTORY_SIZE 128

struct list_clients {
	int count_client;
	int client_id[NUM_CLIENTS];
	char name_client[NUM_CLIENTS][SIZE_TEXT];
};

struct msgbuf {
	long mtype;
	int type_msg;
	int client_id;
	char sender_name[SIZE_TEXT];
	char text[SIZE_TEXT];
	char name_client[NUM_CLIENTS][SIZE_TEXT];
	int count_client;
};

struct chat_msg {
	int count_msg;
	char sender_name[CHAT_HISTORY_SIZE][SIZE_TEXT];
	char text_msg[CHAT_HISTORY_SIZE][SIZE_TEXT];
};

int msgid;
key_t key;

void *exit_server(void *arg)
{
	char buffer[SIZE_TEXT] = {0};
	struct msgbuf msg_exit_server = {0};

	printf("Для завершения программы введитe \"exit\"\n");

	while(1)
	{
		memset(buffer, 0, sizeof(buffer));

		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		if (strcmp(buffer, "exit") == 0)
		{
			msg_exit_server.mtype = MSG_SERVER;
			msg_exit_server.type_msg = SERVER_EXIT;
			msgsnd(msgid, &msg_exit_server, sizeof(msg_exit_server) - sizeof(long), 0);
			break;
		}
	}

	pthread_exit(NULL);
}

int main()
{
	pthread_t thread;
	struct msgbuf msg_rcv;
	struct msgbuf msg_snd;
	struct list_clients list_clients = {0};
	struct chat_msg  history_chat = {0};
	char name_exit_client[SIZE_TEXT] = {0};
	int flag_exit = 1;
	int new_id = 0;

	key = ftok("server.c", 1);

	msgid = msgget(key, 0666);
	if (msgid != -1)
	{
		printf("Очередь уже существует, удаляю\n");
		if (msgctl(msgid, IPC_RMID, NULL) == -1)
		{
			return 1;
		}
	}

	msgid = msgget(key, 0666 | IPC_CREAT);

	if (pthread_create(&thread, NULL, exit_server, NULL) != 0)
	{
		printf("pthread_create\n");
		return 1;
	}

	while (flag_exit)
	{
		memset(&msg_rcv, 0, sizeof(msg_rcv));
		memset(&msg_snd, 0, sizeof(msg_snd));

		msgrcv(msgid, &msg_rcv, sizeof(msg_rcv) - sizeof(long), MSG_SERVER, 0);
		printf("Пришло сообщение с типом %d\n", msg_rcv.type_msg);

		switch (msg_rcv.type_msg)
		{
			case REGISTRATION:
				list_clients.client_id[list_clients.count_client] = CLIENT_ID + new_id++;
				strncpy(list_clients.name_client[list_clients.count_client], msg_rcv.sender_name, sizeof(list_clients.name_client));

				msg_snd.mtype = CONFIRM_REG;
				msg_snd.client_id = list_clients.client_id[list_clients.count_client];
				printf("119 - send %ld\n", msg_snd.mtype);
				msgsnd(msgid, &msg_snd, sizeof(msg_snd) - sizeof(long), 0);

				for (int i = 0; i < history_chat.count_msg; i++)
				{
					msg_snd.mtype = list_clients.client_id[list_clients.count_client];
					msg_snd.type_msg = MSG_CHAT;
					memcpy(msg_snd.sender_name, history_chat.sender_name[i], sizeof(msg_snd.sender_name));
					memcpy(msg_snd.text, history_chat.text_msg[i], sizeof(msg_snd.text));
					printf("%s %s\n", msg_snd.sender_name, msg_snd.text);
					msgsnd(msgid, &msg_snd, sizeof(msg_snd) - sizeof(long), 0);
				}
				list_clients.count_client++;

				memcpy(msg_snd.name_client, list_clients.name_client, sizeof(msg_snd.name_client));

				for (int i = 0; i < list_clients.count_client; i++)
				{
					msg_snd.mtype = list_clients.client_id[i];
					msg_snd.type_msg = NEW_LIST_CLIENTS;
					msg_snd.count_client = list_clients.count_client;
					snprintf(msg_snd.text, sizeof(msg_snd.text), "[SERVER]: connected %s", list_clients.name_client[list_clients.count_client - 1]);
					printf("140 - send %ld\n", msg_snd.mtype);
					msgsnd(msgid, &msg_snd, sizeof(msg_snd) - sizeof(long), 0);
				}
				break;

			case MSG_CHAT:
				if (history_chat.count_msg < CHAT_HISTORY_SIZE)
				{
					strncpy(history_chat.sender_name[history_chat.count_msg], msg_rcv.sender_name, sizeof(history_chat.sender_name[history_chat.count_msg]));
					strncpy(history_chat.text_msg[history_chat.count_msg], msg_rcv.text, SIZE_TEXT);
					history_chat.count_msg++;
				}

				for(int i = 0; i < list_clients.count_client; i++)
				{
					msg_snd = msg_rcv;
					msg_snd.mtype = list_clients.client_id[i];
					printf("157 - %ld\n", msg_snd.mtype);
					msgsnd(msgid, &msg_snd, sizeof(msg_snd) - sizeof(long), 0);
				}
				break;

			case CLIENT_EXIT:
				memset(name_exit_client, 0, sizeof(name_exit_client));

				for (int i = 0; i < list_clients.count_client; i++)
				{
					if (msg_rcv.client_id == list_clients.client_id[i])
					{
						strncpy(name_exit_client, list_clients.name_client[i], sizeof(name_exit_client));

						for (int j = i; j < list_clients.count_client; j++)
						{
							list_clients.client_id[j] = list_clients.client_id[j + 1];
							strncpy(list_clients.name_client[j], list_clients.name_client[j + 1], sizeof(list_clients.name_client[j]));
						}
						list_clients.count_client--;
						break;
					}
				}

				memcpy(msg_snd.name_client, list_clients.name_client, sizeof(msg_snd.name_client));

				for (int i = 0; i < list_clients.count_client; i++)
				{
					msg_snd.mtype = list_clients.client_id[i];
					msg_snd.type_msg = NEW_LIST_CLIENTS;
					msg_snd.count_client = list_clients.count_client;
					snprintf(msg_snd.text, sizeof(msg_snd.text), "[SERVER]: disconnected %s", name_exit_client);
					printf("189 - %ld\n", msg_snd.mtype);
					msgsnd(msgid, &msg_snd, sizeof(msg_snd) - sizeof(long), 0);
				}
				break;

			case SERVER_EXIT:
				flag_exit = 0;
				break;
			}

	}

	pthread_join(thread, NULL);
	msgctl(msgid, IPC_RMID, NULL);

	return 0;
}