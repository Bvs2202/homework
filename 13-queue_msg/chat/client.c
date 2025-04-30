#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <ncurses.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>

#define REGISTRATION 10
#define CONFIRM_REG 11
#define MSG_SERVER 12
#define MSG_CHAT 13
#define NEW_LIST_CLIENTS 14
#define CLIENT_EXIT 15

#define SIZE_TEXT 64
#define NUM_CLIENTS 16
#define CLIENT_ID 100
#define NUM_THREADS 2

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

key_t key;
int msgid = 0;
int client_id = 1;
struct msgbuf msg_read = {0};
struct msgbuf msg_input = {0};
WINDOW *wnd_chat;
WINDOW *wnd_input;
WINDOW *wnd_clients;
int x_chat = 1, y_chat = 1;
int x_clients = 1, y_clients = 1;

void sig_winch(int signo)
{
	struct winsize size;

	ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
	resizeterm(size.ws_row, size.ws_col);
}

void *reg_client(void *arg)
{
	int flag_exit_func = 0;

	while (flag_exit_func == 0)
	{
		msgrcv(msgid, &msg_read, sizeof(msg_read) - sizeof(long), CONFIRM_REG, 0);
		client_id = msg_read.client_id;
		flag_exit_func = 1;
	}

	pthread_exit(NULL);
}

void *read_chat(void *arg)
{
	while (1)
	{
		msgrcv(msgid, &msg_read, sizeof(msg_read) - sizeof(long), client_id, 0);

		switch(msg_read.type_msg)
		{
			case NEW_LIST_CLIENTS:
				mvwprintw(wnd_chat, y_chat, x_chat, "%s", msg_read.text);
				y_chat++;

				werase(wnd_clients);
				box(wnd_clients, 0, 0);
				y_clients = 1;
				x_clients = 1;
				for (int i = 0; i < msg_read.count_client; i++)
				{
					mvwprintw(wnd_clients, y_clients, x_clients, "%s", msg_read.name_client[i]);
					y_clients++;
				}
				wrefresh(wnd_clients);
				break;

			default:
				mvwprintw(wnd_chat, y_chat, x_chat, "[%s]: %s", msg_read.sender_name, msg_read.text);
				y_chat++;
				break;
		}
		wrefresh(wnd_chat);

		mvwprintw(wnd_input, 1, 1, "> ");
		wmove(wnd_input, 1, 3);
		wrefresh(wnd_input);
	}

	pthread_exit(NULL);
}

int main()
{
	pthread_t thread[NUM_THREADS];

	setlocale(LC_ALL, "");
	initscr();
	signal(SIGWINCH, sig_winch);
	curs_set(1);
	refresh();

	wnd_chat = newwin(30, 70, 0, 0);
	wnd_input = newwin(3, 90, 31, 0);
	wnd_clients = newwin(30, 20, 0, 71);
	
	box(wnd_chat, 0, 0);
	box(wnd_input, 0, 0);
	box(wnd_clients, 0, 0);
	wrefresh(wnd_chat);
	wrefresh(wnd_input);
	wrefresh(wnd_clients);

	key = ftok("server.c", 1);

	msgid = msgget(key, 0666);

	mvwprintw(wnd_input, 1, 1, "> name ");
	refresh();
	wrefresh(wnd_input);
	wgetnstr(wnd_input, msg_input.sender_name, sizeof(msg_input.sender_name));
	msg_input.mtype = MSG_SERVER;
	msg_input.type_msg = REGISTRATION;
	msgsnd(msgid, &msg_input, sizeof(msg_input) - sizeof(long), 0);

	if (pthread_create(&thread[0], NULL, reg_client, NULL) != 0)
	{
		printf("pthread_create\n");
		return 1;
	}
	pthread_join(thread[0], NULL);

	if (pthread_create(&thread[1], NULL, read_chat, NULL) != 0)
	{
		printf("pthread_create\n");
		return 1;
	}

	while (1)
	{
		werase(wnd_input);
		box(wnd_input, 0, 0);
		mvwprintw(wnd_input, 1, 1, "> ");
		wrefresh(wnd_input);
		wgetnstr(wnd_input, msg_input.text, sizeof(msg_input.text));

		if (strcmp(msg_input.text, "exit") == 0)
		{
			pthread_cancel(thread[0]);
			pthread_join(thread[0], NULL);
			msg_input.mtype = MSG_SERVER;
			msg_input.type_msg = CLIENT_EXIT;
			msg_input.client_id = client_id;
			msgsnd(msgid, &msg_input, sizeof(msg_input) - sizeof(long), 0);
			break;
		}

		msg_input.mtype = MSG_SERVER;
		msg_input.type_msg = MSG_CHAT;
		msg_input.client_id = client_id;
		msgsnd(msgid, &msg_input, sizeof(msg_input) - sizeof(long), 0);
	}

	delwin(wnd_chat);
	delwin(wnd_input);
	delwin(wnd_clients);
	curs_set(0);
	endwin();

	return 0;
}