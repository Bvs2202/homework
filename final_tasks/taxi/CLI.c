#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/select.h>
#include <signal.h>
#include <time.h>

#define MAX_DRIVERS 32
#define MAX_CMD_LEN 32
#define BUFFER_SIZE 64

enum Status {
	AVAILABLE_STATUS = 0,
	BUSY_STATUS = 1
};

struct Driver {
	pid_t pid;
	int fd_cli;
	int fd_driver;
	enum Status status;
	unsigned int task_timer;
};

int has_input()
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	return select(STDIN_FILENO + 1, &fds, NULL, NULL, NULL) > 0;
}

int create_driver(struct Driver *drivers)
{
	int pid = 0;
	int sv[2] = {0};
	char cmd_input[MAX_CMD_LEN] = {0};
	char cmd_name[MAX_CMD_LEN] = {0};
	char buff[BUFFER_SIZE] = {0};
	int arg1 = 0;
	int arg2 = 0;
	time_t rem_time, start_time, curr_time;

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) == -1)
	{
		perror("socketpair");
		return -1;
	}

	drivers->task_timer = 0;
	drivers->status = AVAILABLE_STATUS;
	drivers->fd_driver = sv[1];
	drivers->fd_cli = sv[0];
	drivers->pid = fork();

	if (drivers->pid == -1)
	{
		perror("fork");
		return -1;
	}
	else if (drivers->pid == 0)
	{
		close(sv[0]);
		printf("create driver pid = %d\n", getpid());

		while(1)
		{
			memset(cmd_name, 0, sizeof(cmd_name));
			memset(cmd_input, 0, sizeof(cmd_input));
			arg1 = 0;
			arg2 = 0;
			read(sv[1], cmd_input, MAX_CMD_LEN);

			sscanf(cmd_input, "%32s %d %d", cmd_name, &arg1, &arg2);

			if (drivers->status == BUSY_STATUS)
			{
				curr_time = time(NULL) - start_time;
				if (drivers->task_timer <= curr_time)
					drivers->status = AVAILABLE_STATUS;
			}

			if (strcmp("send_task", cmd_name) == 0)
			{
				if (drivers->status == AVAILABLE_STATUS)
				{
					start_time = time(NULL);
					drivers->task_timer = arg2;
					drivers->status = BUSY_STATUS;
				}
				else if (drivers->status == BUSY_STATUS)
				{
					rem_time = drivers->task_timer - (time(NULL) - start_time);
					memset(buff, 0, sizeof(buff));
					snprintf(buff, BUFFER_SIZE, "Busy %lu", rem_time);
					write(sv[1], buff, strlen(buff));
				}
			}
			else if (strcmp("get_status", cmd_name) == 0)
			{
				memset(buff, 0, sizeof(buff));
				if (drivers->status == AVAILABLE_STATUS)
				{
					snprintf(buff, BUFFER_SIZE, "%s", "Available");
				}
				else if (drivers->status == BUSY_STATUS)
				{
					rem_time = drivers->task_timer - (time(NULL) - start_time);
					snprintf(buff, BUFFER_SIZE, "Busy %lu", rem_time);
				}
				write(sv[1], buff, strlen(buff));
			}
		}
	}
	else
	{
		close(sv[1]);
	}

	return 0;
}

int main()
{
	struct Driver drivers[MAX_DRIVERS] = {0};
	int n = 0;
	char cmd_input[MAX_CMD_LEN] = {0};
	char cmd_name[MAX_CMD_LEN] = {0};
	char buff[BUFFER_SIZE] = {0};
	int arg1 = 0;
	int arg2 = 0;
	int fd_max = 0;
	fd_set fds;

	printf("Commands:\n");
	printf("   create_driver\n");
	printf("   send_task <pid> <task_timer>\n");
	printf("   get_status <pid>\n");
	printf("   get_drivers\n\n");

	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		for (int i = 0; i < MAX_DRIVERS; i++)
		{
			if (drivers[i].fd_cli != 0)
			{
				FD_SET(drivers[i].fd_cli, &fds);
				if (drivers[i].fd_cli > fd_max)
					fd_max = drivers[i].fd_cli;
			}
		}

		if (select(fd_max + 1, &fds, NULL, NULL, NULL) == 0)
			continue;

		if (FD_ISSET(STDIN_FILENO, &fds))
		{
			memset(cmd_input, 0, sizeof(cmd_input));
			fgets(cmd_input, sizeof(cmd_input), stdin);
			cmd_input[strcspn(cmd_input, "\n")] = 0;

			if (strcmp(cmd_input, "exit") == 0)
			{
				break;
			}

			memset(cmd_name, 0, sizeof(cmd_name));
			arg1 = 0;
			arg2 = 0;

			n = sscanf(cmd_input, "%32s %d %d", cmd_name, &arg1, &arg2);

			if (n >= 1)
			{
				if (strcmp("create_driver", cmd_name) == 0)
				{
					for (int i = 0; i < MAX_DRIVERS; i++)
					{
						if (drivers[i].pid == 0)
						{
							if (create_driver(&(drivers[i])) == -1)
							{
								printf("Error create_driver\n");
								perror("create_driver");
							}
							else
							{
								close(drivers[i].fd_driver);
							}

							break;
						}
					}
				}
				else if (strcmp("send_task", cmd_name) == 0 && n == 3)
				{
					int flag = 0;
					for (int i = 0; i < MAX_DRIVERS; i++)
					{
						if (drivers[i].pid == arg1)
						{
							flag = 1;
							write(drivers[i].fd_cli, cmd_input, strlen(cmd_input));
							break;
						}
					}
					if (flag == 0)
					{
						printf("The pid was not found.\n");
					}
				}
				else if (strcmp("get_status", cmd_name) == 0)
				{
					int flag = 0;
					for (int i = 0; i < MAX_DRIVERS; i++)
					{
						if (drivers[i].pid == arg1)
						{
							flag = 1;
							write(drivers[i].fd_cli, "get_status", strlen("get_status"));
							break;
						}
					}
					if (flag == 0)
					{
						printf("The pid was not found.\n");
					}
				}
				else if (strcmp("get_drivers", cmd_name) == 0)
				{
					for (int i = 0; i < MAX_DRIVERS; i++)
					{
						if (drivers[i].pid != 0)
							write(drivers[i].fd_cli, "get_status", strlen("get_status"));
					}
				}
				else
				{
					printf("Wrong command!\n");
					continue;
				}
			}
			else
			{
				printf("The command is not entered\n");
			}
		}

		for (int i = 0; i < MAX_DRIVERS; i++)
		{
			if (drivers[i].pid != 0 &&
				FD_ISSET(drivers[i].fd_cli, &fds))
			{
				memset(buff, 0, sizeof(buff));
				if (read(drivers[i].fd_cli, buff, sizeof(buff)) >= 0)
					printf("index %d pid %d fd %d msg: %s\n",
							i, drivers[i].pid, drivers[i].fd_cli, buff);
			}
		}
	}

	for (int i = 0; i < MAX_DRIVERS; i++)
	{
		if (drivers[i].pid != 0)
		{
			close(drivers[i].fd_cli);
			kill(drivers[i].pid, SIGTERM);
			waitpid(drivers[i].pid, NULL, 0);
		}
	}

	printf("Completion of the program, good bye!\n");
	
	return 0;
}