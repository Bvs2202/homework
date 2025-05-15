#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <termios.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "common.h"

#define SHM_NAME "/chat_shm"
#define SEM_NAME "/chat_mutex"
#define SEM_CLIENT "/chat_client"
#define SEM_NEW_MSG "/chat_new_msg"
#define SEM_USER "/chat_user"

#define NUM_THREADS 2

int shm_fd;
int id_user;
struct chat *chat;
sem_t *sem_mutex, *sem_client, *sem_new_msg, *sem_user;
pthread_t thread[NUM_THREADS];

WINDOW *wnd_chat;
WINDOW *wnd_input;
WINDOW *wnd_clients;
int x_chat = 1, y_chat = 1;
int x_clients = 1, y_clients = 1;
char msg_input[MAX_MSG_LEN];
char name[MAX_NAME];

void sig_winch(int signo)
{
	struct winsize size;

	ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
	resizeterm(size.ws_row, size.ws_col);
}

void *read_chat(void *arg)
{
	while (1)
	{
		sem_wait(sem_new_msg);
		
		sem_wait(sem_mutex);
		while (chat->users[id_user].last_msg_read != chat->count_history)
		{
			mvwprintw(wnd_chat, y_chat, x_chat, "[%s]: %s", chat->history[chat->users[id_user].last_msg_read].name, 
				chat->history[chat->users[id_user].last_msg_read].text);
			y_chat++;
			chat->users[id_user].last_msg_read++;
		}
		wrefresh(wnd_chat);
		sem_post(sem_mutex);

		werase(wnd_input);
		box(wnd_input, 0, 0);
		mvwprintw(wnd_input, 1, 1, "> ");
		wrefresh(wnd_input);	

		usleep(1000);
	}

	pthread_exit(NULL);
}

void *change_users(void *arg)
{
	while (1)
	{
		sem_wait(sem_user);

		sem_wait(sem_mutex);
		werase(wnd_clients);
		box(wnd_clients, 0, 0);
		y_clients = 1;
		x_clients = 1;
		for (int i = 0; i < chat->count_users; i++)
		{
			if (chat->users[i].active == 1)
			{
				mvwprintw(wnd_clients, y_clients, x_clients, "%s", chat->users[i].name);
				y_clients++;
			}
		}
		wrefresh(wnd_clients);
		sem_post(sem_mutex);

		werase(wnd_input);
		box(wnd_input, 0, 0);
		mvwprintw(wnd_input, 1, 1, "> ");
		wrefresh(wnd_input);

		usleep(1000);
	}

	pthread_exit(NULL);
}

void programm_end(int signum)
{
	sem_wait(sem_mutex);
	chat->last_type_msg = EXIT_USER;
	chat->last_id_user = id_user;
	sem_post(sem_mutex);
	sem_post(sem_client);

	munmap(chat, sizeof(struct chat));
	close(shm_fd);
	sem_close(sem_mutex);
	sem_close(sem_client);
	sem_close(sem_new_msg);
	sem_close(sem_user);

	printf("\n\nПрограмма завершена\n\n");

	pthread_cancel(thread[0]);
	pthread_cancel(thread[1]);
	endwin();

	fflush(stdout);

	exit(0);
}

int main()
{
	signal(SIGINT, programm_end);

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


	if ((shm_fd = shm_open(SHM_NAME, O_RDWR, 0)) == -1)
	{
		programm_end(0);
		perror("shm_open");
		exit(1);
	}

	if (ftruncate(shm_fd, sizeof(struct chat)) == -1)
	{
		programm_end(0);
		perror("ftruncate");
		exit(1);
	}

	chat = mmap(NULL, sizeof(struct chat), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (chat == MAP_FAILED)
	{
		programm_end(0);
		perror("mmap");
		exit(1);
	}

	if ((sem_mutex = sem_open(SEM_NAME, O_RDWR, 0666)) == SEM_FAILED)
	{
		programm_end(0);
		perror("sem_open");
		exit(1);
	}

	if ((sem_client = sem_open(SEM_CLIENT, O_RDWR, 0666)) == SEM_FAILED)
	{
		programm_end(0);
		perror("sem_open");
		exit(1);
	}

	if ((sem_new_msg = sem_open(SEM_NEW_MSG, O_RDWR, 0666)) == SEM_FAILED)
	{
		programm_end(0);
		perror("sem_open");
		exit(1);
	}

	if ((sem_user = sem_open(SEM_USER, O_RDWR, 0666)) == SEM_FAILED)
	{
		programm_end(0);
		perror("sem_open");
		exit(1);
	}

	mvwprintw(wnd_input, 1, 1, "> name ");
	refresh();
	wrefresh(wnd_input);
	wgetnstr(wnd_input, name, MAX_NAME);

	sem_wait(sem_mutex);
	for (int i = 0; i < MAX_USERS; i++)
	{
		if (chat->users[i].active == 0)
		{
			strncpy(chat->users[i].name, name, MAX_NAME);
			chat->users[i].active = 1;
			chat->last_id_user = i;
			chat->last_type_msg = NEW_USER;
			id_user = i;
			break;
		}
	}

	while (chat->users[id_user].last_msg_read != chat->count_history)
	{
		mvwprintw(wnd_chat, y_chat, x_chat, "[%s]: %s", chat->history[chat->users[id_user].last_msg_read].name, 
			chat->history[chat->users[id_user].last_msg_read].text);
		y_chat++;
		chat->users[id_user].last_msg_read++;
	}
	wrefresh(wnd_chat);

	werase(wnd_input);
	box(wnd_input, 0, 0);
	mvwprintw(wnd_input, 1, 1, "> ");
	wrefresh(wnd_input);

	sem_post(sem_mutex);
	sem_post(sem_client);

	if (pthread_create(&thread[0], NULL, read_chat, NULL) != 0)
	{
		programm_end(0);
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&thread[1], NULL, change_users, NULL) != 0)
	{
		programm_end(0);
		perror("pthread_create");
		exit(1);
	}

	while (1)
	{
		werase(wnd_input);
		box(wnd_input, 0, 0);
		mvwprintw(wnd_input, 1, 1, "> ");
		wrefresh(wnd_input);
		wgetnstr(wnd_input, msg_input, MAX_MSG_LEN);

		if (strcmp(msg_input, "exit") == 0)
		{
			break;
		}

		sem_wait(sem_mutex);
		strncpy(chat->history[chat->count_history].name, name, MAX_NAME);
		strncpy(chat->history[chat->count_history].text, msg_input, MAX_MSG_LEN);
		chat->last_type_msg = NEW_MSG;
		chat->count_history++;
		sem_post(sem_mutex);
		sem_post(sem_client);
	}

	programm_end(0);

	return 0;
}