#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>

#include "common.h"

#define SHM_NAME "/chat_shm"
#define SEM_NAME "/chat_mutex"
#define SEM_CLIENT "/chat_client"
#define SEM_NEW_MSG "/chat_new_msg"
#define SEM_USER "/chat_user"

int shm_fd;
struct chat *chat;
sem_t *sem_mutex, *sem_client, *sem_new_msg, *sem_user;

void programm_end(int signum)
{
	munmap(chat, sizeof(struct chat));
	close(shm_fd);
	sem_close(sem_mutex);
	sem_close(sem_client);
	sem_close(sem_new_msg);
	sem_close(sem_user);
	sem_unlink(SEM_NAME);
	sem_unlink(SEM_CLIENT);
	sem_unlink(SEM_NEW_MSG);
	sem_unlink(SEM_USER);
	shm_unlink(SHM_NAME);

	printf("\n\nПрограмма завершена\n\n");

	fflush(stdout);

	exit(0);
}

int main()
{
	signal(SIGINT, programm_end);

	if ((shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, 0666)) == -1)
	{
		perror("shm_open");
		exit(1);
	}

	printf("%ld\n", sizeof(struct chat));
	if (ftruncate(shm_fd, sizeof(struct chat)) == -1)
	{
		perror("ftruncate");
		exit(1);
	}

	chat = mmap(NULL, sizeof(struct chat), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (chat == MAP_FAILED)
	{
		perror("mmap");
		exit(1);
	}

	if ((sem_mutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
	{
		perror("sem_open");
		exit(1);
	}

	if ((sem_client = sem_open(SEM_CLIENT, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
	{
		perror("sem_open");
		exit(1);
	}

	if ((sem_new_msg = sem_open(SEM_NEW_MSG, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
	{
		perror("sem_open");
		exit(1);
	}

	if ((sem_user = sem_open(SEM_USER, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
	{
		perror("sem_open");
		exit(1);
	}

	memset(chat, 0, sizeof(struct chat));

	printf("Для завершения программы зажмите Ctrl+C\n\n");

	while (1)
	{
		sem_wait(sem_client);
		printf("Пришло сообщение типа %d\n", chat->last_type_msg);

		switch (chat->last_type_msg)
		{
			case NEW_USER:
				sem_wait(sem_mutex);
				strncpy(chat->history[chat->count_history].name, "SERVER", MAX_MSG_LEN);
				strncpy(chat->history[chat->count_history].text, "connected ", MAX_MSG_LEN);
				strncat(chat->history[chat->count_history].text, chat->users[chat->count_history].name, MAX_MSG_LEN);
				chat->count_history++;
				chat->count_users++;
				sem_post(sem_mutex);

				for (int i = 0; i < chat->count_users; i++)
				{
					sem_post(sem_user);
					sem_post(sem_new_msg);
				}
				break;

			case NEW_MSG:
				sem_wait(sem_mutex);
				for (int i = 0; i < chat->count_users; i++)
				{
					sem_post(sem_new_msg);
				}
				sem_post(sem_mutex);
				break;

			case EXIT_USER:
				sem_wait(sem_mutex);
				chat->count_users--;
				chat->users[chat->last_id_user].active = 0;
				chat->users[chat->last_id_user].last_msg_read = 0;

				for (int i = 0; i < chat->count_users; i++)
				{
					sem_post(sem_user);
				}
				sem_post(sem_mutex);
				break;
		}
	}

	return 0;
}